///<reference path="./crpl.d.ts" />

let l = [1, "Hello", 3 + 5];

function main() {
    let a = l[2];
    l[0] = l[1] = 32;
    l[2 + 2] = 5;
}
