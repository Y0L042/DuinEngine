options gen2
options rtti

require fio
require rtti
require ast
require daslib/ast_boost
require daslib/cpp_bind public

module gen_adapter

def write_adapter(cpp_file : file; name : string; cinfo : TypeDeclPtr) {
    log_cpp_class_adapter(cpp_file, name, cinfo)
}