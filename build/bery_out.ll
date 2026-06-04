
define i32 @main() {
entry:
    %a = alloca i32
    %1 = add i32 0, 10
    store i32 %1, i32* %a
    ret i32 0
}
