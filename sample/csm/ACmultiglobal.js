///<reference path="../crpl.d.ts" />

const DefaultPower = 1.0;
const Name = "acmultiglobal";

let Powers = [];
let NegaPowers = [];

let MultiPowers = [];
let NegaMultiPowers = [];

let CChanged = false;
let Changed = false;

function main() {
    if (Changed) {
        let TotalPower = DefaultPower;

        for (const item of Powers) { TotalPower = TotalPower + item; }
        for (const item of MultiPowers) { TotalPower = TotalPower * item; }

        for (const Target of GetCoresWithVar("HasEmitterInterface", 1)) {
            let EmitterType = GetScriptVar(Target, "EmitterInterface.crpl", "EmitterType");
            SetScriptVar(Target, `${EmitterType}.crpl`, "TotalPower", TotalPower);
        }

        Changed = false;
    }

    if(CChanged) {
        let NegaPower = 1;

        for (const item of NegaPowers) { NegaPower = NegaPower + item; }
        for (const item of NegaMultiPowers) { NegaPower = NegaPower * item; }

        for (const Target of GetCoresWithVar("HasEmitterInterface", 1)) {
            SetScriptVar(Target, `${GetScriptVar(Target, "EmitterInterface.crpl", "EmitterType")}.crpl`, "NegaPower", NegaPower);
        }

        CChanged = false;
    }
}
