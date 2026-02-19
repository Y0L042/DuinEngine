"""
Process raw XML coverage data and output a cleaned, neat version showing
lines covered, not covered, and partially covered counts per function,
grouped by class/namespace, with long template names shortened.
"""

import sys
import xml.etree.ElementTree as ET
from html import unescape
from collections import defaultdict

# ----------------------------------------------------------------------
# Configuration
# ----------------------------------------------------------------------
TRUNCATE_LENGTH = 80          # max length for type names before truncation
PREFIX_KEEP = 50               # characters to keep from start when truncating
SUFFIX_KEEP = 30               # characters to keep from end when truncating

# ----------------------------------------------------------------------
# Helper functions
# ----------------------------------------------------------------------
def unescape_html(text):
    """Convert HTML entities to characters."""
    return unescape(text)

def shorten_type_name(type_name):
    """
    Shorten extremely long type names by truncating with ellipsis.
    First unescapes HTML entities.
    """
    if not type_name:
        return ""
    cleaned = unescape_html(type_name)
    if len(cleaned) <= TRUNCATE_LENGTH:
        return cleaned
    # Simple truncation: keep first PREFIX_KEEP and last SUFFIX_KEEP chars
    return cleaned[:PREFIX_KEEP] + "..." + cleaned[-SUFFIX_KEEP:]

def format_function_name(name, type_name):
    """
    Format the function name for display.
    If type_name is present, show as "name (shortened_type)".
    Otherwise just name.
    """
    if type_name:
        short_type = shorten_type_name(type_name)
        return f"{name} ({short_type})"
    else:
        return name

def calculate_coverage_percent(covered, not_covered, partial):
    """Calculate coverage percentage."""
    total = covered + not_covered + partial
    if total == 0:
        return 0.0
    return (covered + partial * 0.5) / total * 100

# ----------------------------------------------------------------------
# Parsing
# ----------------------------------------------------------------------
def parse_coverage_data(xml_string):
    """Parse XML and return list of function dicts."""
    root = ET.fromstring(xml_string)
    functions = []
    for elem in root.findall(".//function"):
        func = {
            "name": elem.get("name", ""),
            "namespace": elem.get("namespace", ""),
            "type_name": elem.get("type_name", ""),
            "lines_covered": int(elem.get("lines_covered", 0)),
            "lines_not_covered": int(elem.get("lines_not_covered", 0)),
            "lines_partially_covered": int(elem.get("lines_partially_covered", 0)),
        }
        functions.append(func)
    return functions

# ----------------------------------------------------------------------
# Grouping and sorting
# ----------------------------------------------------------------------
def group_functions(functions):
    """Group functions by (namespace, type_name)."""
    groups = defaultdict(list)
    for func in functions:
        key = (func["namespace"], func["type_name"])
        groups[key].append(func)
    return groups

def sort_groups(groups):
    """Return sorted list of group keys."""
    return sorted(groups.keys(), key=lambda k: (k[0], k[1]))

def sort_functions(func_list):
    """Sort functions by name."""
    return sorted(func_list, key=lambda f: f["name"])

# ----------------------------------------------------------------------
# Output
# ----------------------------------------------------------------------
def write_group_table(group_key, func_list, out_file=sys.stdout):
    """Write a markdown table for a single group."""
    namespace, type_name = group_key

    # Determine group title
    if type_name:
        if namespace:
            title = f"{namespace}::{type_name}"
        else:
            title = type_name
    else:
        title = namespace if namespace else "Global"

    out_file.write(f"\n## {title}\n\n")
    out_file.write("| Function | Covered | Not Covered | Partial | Coverage % |\n")
    out_file.write("|----------|--------:|------------:|--------:|----------:|\n")

    totals = {"covered": 0, "not_covered": 0, "partial": 0}

    for func in sort_functions(func_list):
        display_name = format_function_name(func["name"], func["type_name"])
        covered = func["lines_covered"]
        not_covered = func["lines_not_covered"]
        partial = func["lines_partially_covered"]

        totals["covered"] += covered
        totals["not_covered"] += not_covered
        totals["partial"] += partial

        coverage_pct = calculate_coverage_percent(covered, not_covered, partial)

        out_file.write(f"| {display_name} | {covered} | {not_covered} | {partial} | {coverage_pct:.1f}% |\n")

    # Write group summary
    total_coverage = calculate_coverage_percent(totals["covered"], totals["not_covered"], totals["partial"])
    out_file.write(f"| **TOTAL** | **{totals['covered']}** | **{totals['not_covered']}** | **{totals['partial']}** | **{total_coverage:.1f}%** |\n")

def main():
    # Read input (stdin or first argument)
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r', encoding='utf-8') as f:
            xml_data = f.read()
    else:
        xml_data = sys.stdin.read()

    # Parse
    try:
        functions = parse_coverage_data(xml_data)
    except ET.ParseError as e:
        print(f"Error parsing XML: {e}", file=sys.stderr)
        sys.exit(1)

    # Group
    groups = group_functions(functions)
    sorted_keys = sort_groups(groups)

    # Output header
    print("# Coverage Summary\n")

    # Write each group
    for key in sorted_keys:
        write_group_table(key, groups[key])

if __name__ == "__main__":
    main()