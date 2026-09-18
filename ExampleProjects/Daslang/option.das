options gen2
options log
require daslib/option

struct Test {
    i: int
}

[export]
def main() {
    // var test: Option<Test> = some(Test(i=5))
    var test: Option<Test> = none(type<Test>)
    // var test2 = test |> unwrap_or(Test(i=69))
    // var test2 = test.unwrap_or(Test(i=69))
    var test2 = test.and_then() $(test: Test) { return Test(i=test.i*10); }

    print("test2.i: {test2.unwrap_or(Test(i=0))}")
}
