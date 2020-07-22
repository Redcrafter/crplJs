
const emitAmount = 20;
const emitDelay = 15;

const sporeAmount = 10;
const sporeDelay = 60;
const sporeCount = 10;
const sporeRadius = 20;

SetTimer0(emitDelay);
SetTimer1(sporeDelay);

function main() {
    if(GetTimer0() == 0) {
        SetTimer0(emitDelay);

        SetCreeperNoLower(CurrentCoords, emitAmount);
    }

    if(GetTimer1() == 0) {
        SetTimer1(sporeDelay);

        for(let i = 0; i < sporeCount; i++) {
            let [x, y] = RandCoordsInRange(CurrentCoords, sporeRadius);

            CreateSpore(CurrentCoords, x, y, 1, sporeAmount);
        }
    }
}
