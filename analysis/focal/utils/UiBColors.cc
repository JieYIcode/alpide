#include "TColor.h"

float Z = 256.;

TColor *uibDigitalRed = new TColor(TColor::GetFreeColorIndex(), 207/Z, 60/Z, 58/Z, "uibDigitalRed");
TColor *uibDigitalBlue = new TColor(TColor::GetFreeColorIndex(), 78/Z, 160/Z, 183/Z, "uibDigitalBlue");
TColor *uibDigitalGreen = new TColor(TColor::GetFreeColorIndex(), 120/Z, 154/Z, 91/Z, "uibDigitalGreen");
TColor *uibOceanBlack = new TColor(TColor::GetFreeColorIndex(), 51/Z, 51/Z, 51/Z, "uibOceanBlack");
TColor *uibOceanBlue = new TColor(TColor::GetFreeColorIndex(), 0/Z, 117/Z, 175/Z, "uibOceanBlue");
TColor *ROOTBeer = new TColor(TColor::GetFreeColorIndex(), 41/Z, 14/Z, 5/Z, "ROOTBeer");
TColor *uibOrange = new TColor(TColor::GetFreeColorIndex(), 245/Z, 186/Z, 0/Z, "UiBOrange");
TColor *uibViolet = new TColor(TColor::GetFreeColorIndex(), 82/Z, 45/Z, 137/Z, "uibViolet");
TColor *uibMattBlau = new TColor(TColor::GetFreeColorIndex(), 92/Z, 134/Z, 197/Z, "uibMattBlau");
TColor *uibLightGreen= new TColor(TColor::GetFreeColorIndex(), 119/Z, 175/Z, 0/Z, "uibMattBlau");


unsigned int uibColorIndexForEnergy(unsigned int energy){
    //return 4;
    switch(energy){
        case 20:
            return uibDigitalRed->GetNumber();
        case 40:
            return uibDigitalBlue->GetNumber();
        case 60:
            return uibDigitalGreen->GetNumber();
        case 80:
            return uibOrange->GetNumber();
        case 100:
            return 96;
        case 150:
            return 52;
        case 200:
            return kMagenta;
        case 250:
            return kBlue;
        case 300:
            return ROOTBeer->GetNumber();
        default:
            return 1;
    }
    return 0;
}

unsigned int uibColorIndex(unsigned int number){
    //return 4;
    switch(number){
        case 0:
            return uibDigitalRed->GetNumber();
        case 1:
            return uibDigitalBlue->GetNumber();
        case 2:
            return uibDigitalGreen->GetNumber();
        case 3:
            return uibOrange->GetNumber();
        case 4:
            return 96;
        case 5:
            return 52;
        case 6:
            return kGreen+1;
        case 7:
            return kBlue;
        case 8:
            return ROOTBeer->GetNumber();
        default:
            return 1;
    }
    return 0;
}


