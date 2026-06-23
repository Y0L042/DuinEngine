REPO_ROOT    := $(shell git -C "$(dir $(abspath $(lastword $(MAKEFILE_LIST))))" rev-parse --show-toplevel)
DASLANG_LIVE := $(REPO_ROOT)/Duin/vendor/daslang/bin/Debug/daslang-live.exe
DASROOT      := $(REPO_ROOT)/Duin/vendor/daslang
PROJECT      := $(REPO_ROOT)/DasProDebugger/dasprodebugger.das_project
MAIN         := $(REPO_ROOT)/DasProDebugger/src/main.das
DAP_PORT     ?= 10000

.PHONY: run run-port help

run:
	"$(DASLANG_LIVE)" -dasroot "$(DASROOT)" -project "$(PROJECT)" "$(MAIN)"

run-port:
	"$(DASLANG_LIVE)" -dasroot "$(DASROOT)" -project "$(PROJECT)" "$(MAIN)" -- --das-debug-port $(DAP_PORT)

help:
	@echo "Targets:"
	@echo "  run           Launch DasProDebugger (DAP on port 10000)"
	@echo "  run-port      Launch with custom port  e.g. make run-port DAP_PORT=10001"
