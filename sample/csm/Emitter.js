///<reference path="../crpl.d.ts" />

const StartDelay = 0;
const EmitInterval = 15;
const EmitAmt = 20;

let Unstopped = 1;

// AC Power
let TotalPower = 1; // Power from global multi
let EmpowermentAC = 1; // Power from Armored Emitter
let ACvar = 1; // Power from Chips
let EmissionBonus = 0; // Bonus from emitter upgrades

// Creeper Power
let NegaPower = 1; // Power from global multi
let Empowerment = 1; // Power from Armored Emitter
let Cvar = 1; // Power from Chips

awake();
delay(4);

function main() {
    let amount;
    if(EmitAmt > 0) {
        amount = EmitAmt * NegaPower * Empowerment * Cvar;
    } else {
        amount = EmitAmt * TotalPower * EmpowermentAC * ACvar - EmissionBonus;
    }

    if(GetGameTimeFrames() > StartDelay) {
        if(amount != 0) {
            SetCreeperNoLower(CurrentCoords, amount);
            if(amount > 0) {
                SetPopupText(`Producing ${amount} creeper every ${round(EmitInterval / 30.0, 1)}s\n\n`);
            } else {
                SetPopupText(`Producing ${-amount} creeper every ${round(EmitInterval / 30.0, 1)}s\n\n`);
            }
        }
        delay(EmitInterval);
    } else {
        SetPopupText(`This emitter is currently slumbering. It will awake in ${ceil((StartDelay - GetGameTimeFrames()) / 30)} seconds with an output of ${amount}\n\n`);
        delay(15);
    }
}

function awake() {
    if (GetScriptVar(Self, "EmitterInterface.crpl", "HasEmitterInterface") == 0) {
        AddScriptToUnit(Self, "EmitterInterface.crpl");
    }

    SetScriptVar(Self, "EmitterInterface.crpl", "EmitterType", "Emitter");

    if(EmitAmt > 0) {
        SetPopupText(`Producing ${EmitAmt} creeper every ${round(EmitInterval / 30.0, 1)}s\n\n`);
    } else {
        SetPopupText(`Producing ${-EmitAmt} creeper every ${round(EmitInterval / 30.0, 1)}s\n\n`);
    }
}
