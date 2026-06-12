options gen2
options rtti
options log

require ./dn_ecs_boost.das

[export]
def main() {
    test()
}

[ecs_system]
def test_system() {
	print("Hello!")
}
