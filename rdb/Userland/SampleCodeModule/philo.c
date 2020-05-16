#include <philo.h>
#include <commands.h>
#include <test_util.h>
#include <lib_user.h>

static int drawing_positions[35][35][2] = {
    {{ 1000, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ -1000, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ -499, 866},{ -500, -866},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 0, 1000},{ -1000, 0},{ 0, -1000},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 309, 951},{ -809, 587},{ -809, -587},{ 309, -951},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 500, 866},{ -499, 866},{ -1000, 0},{ -500, -866},{ 500, -866},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 623, 781},{ -222, 974},{ -900, 433},{ -900, -433},{ -222, -974},{ 623, -781},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 707, 707},{ 0, 1000},{ -707, 707},{ -1000, 0},{ -707, -707},{ 0, -1000},{ 707, -707},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 766, 642},{ 173, 984},{ -499, 866},{ -939, 342},{ -939, -342},{ -500, -866},{ 173, -984},{ 766, -642},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 809, 587},{ 309, 951},{ -309, 951},{ -809, 587},{ -1000, 0},{ -809, -587},{ -309, -951},{ 309, -951},{ 809, -587},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 841, 540},{ 415, 909},{ -142, 989},{ -654, 755},{ -959, 281},{ -959, -281},{ -654, -755},{ -142, -989},{ 415, -909},{ 841, -540},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 866, 499},{ 500, 866},{ 0, 1000},{ -499, 866},{ -866, 499},{ -1000, 0},{ -866, -500},{ -500, -866},{ 0, -1000},{ 500, -866},{ 866, -500},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 885, 464},{ 568, 822},{ 120, 992},{ -354, 935},{ -748, 663},{ -970, 239},{ -970, -239},{ -748, -663},{ -354, -935},{ 120, -992},{ 568, -822},{ 885, -464},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 900, 433},{ 623, 781},{ 222, 974},{ -222, 974},{ -623, 781},{ -900, 433},{ -1000, 0},{ -900, -433},{ -623, -781},{ -222, -974},{ 222, -974},{ 623, -781},{ 900, -433},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 913, 406},{ 669, 743},{ 309, 951},{ -104, 994},{ -499, 866},{ -809, 587},{ -978, 207},{ -978, -207},{ -809, -587},{ -500, -866},{ -104, -994},{ 309, -951},{ 669, -743},{ 913, -406},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 923, 382},{ 707, 707},{ 382, 923},{ 0, 1000},{ -382, 923},{ -707, 707},{ -923, 382},{ -1000, 0},{ -923, -382},{ -707, -707},{ -382, -923},{ 0, -1000},{ 382, -923},{ 707, -707},{ 923, -382},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 932, 361},{ 739, 673},{ 445, 895},{ 92, 995},{ -273, 961},{ -602, 798},{ -850, 526},{ -982, 183},{ -982, -183},{ -850, -526},{ -602, -798},{ -273, -961},{ 92, -995},{ 445, -895},{ 739, -673},{ 932, -361},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 939, 342},{ 766, 642},{ 500, 866},{ 173, 984},{ -173, 984},{ -499, 866},{ -766, 642},{ -939, 342},{ -1000, 0},{ -939, -342},{ -766, -642},{ -500, -866},{ -173, -984},{ 173, -984},{ 500, -866},{ 766, -642},{ 939, -342},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 945, 324},{ 789, 614},{ 546, 837},{ 245, 969},{ -82, 996},{ -401, 915},{ -677, 735},{ -879, 475},{ -986, 164},{ -986, -164},{ -879, -475},{ -677, -735},{ -401, -915},{ -82, -996},{ 245, -969},{ 546, -837},{ 789, -614},{ 945, -324},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 951, 309},{ 809, 587},{ 587, 809},{ 309, 951},{ 0, 1000},{ -309, 951},{ -587, 809},{ -809, 587},{ -951, 309},{ -1000, 0},{ -951, -309},{ -809, -587},{ -587, -809},{ -309, -951},{ 0, -1000},{ 309, -951},{ 587, -809},{ 809, -587},{ 951, -309},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 955, 294},{ 826, 563},{ 623, 781},{ 365, 930},{ 74, 997},{ -222, 974},{ -499, 866},{ -733, 680},{ -900, 433},{ -988, 149},{ -988, -149},{ -900, -433},{ -733, -680},{ -500, -866},{ -222, -974},{ 74, -997},{ 365, -930},{ 623, -781},{ 826, -563},{ 955, -294},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 959, 281},{ 841, 540},{ 654, 755},{ 415, 909},{ 142, 989},{ -142, 989},{ -415, 909},{ -654, 755},{ -841, 540},{ -959, 281},{ -1000, 0},{ -959, -281},{ -841, -540},{ -654, -755},{ -415, -909},{ -142, -989},{ 142, -989},{ 415, -909},{ 654, -755},{ 841, -540},{ 959, -281},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 962, 269},{ 854, 519},{ 682, 730},{ 460, 887},{ 203, 979},{ -68, 997},{ -334, 942},{ -576, 816},{ -775, 631},{ -917, 398},{ -990, 136},{ -990, -136},{ -917, -398},{ -775, -631},{ -576, -816},{ -334, -942},{ -68, -997},{ 203, -979},{ 460, -887},{ 682, -730},{ 854, -519},{ 962, -269},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 965, 258},{ 866, 499},{ 707, 707},{ 500, 866},{ 258, 965},{ 0, 1000},{ -258, 965},{ -499, 866},{ -707, 707},{ -866, 499},{ -965, 258},{ -1000, 0},{ -965, -258},{ -866, -500},{ -707, -707},{ -500, -866},{ -258, -965},{ 0, -1000},{ 258, -965},{ 500, -866},{ 707, -707},{ 866, -500},{ 965, -258},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 968, 248},{ 876, 481},{ 728, 684},{ 535, 844},{ 309, 951},{ 62, 998},{ -187, 982},{ -425, 904},{ -637, 770},{ -809, 587},{ -929, 368},{ -992, 125},{ -992, -125},{ -929, -368},{ -809, -587},{ -637, -770},{ -425, -904},{ -187, -982},{ 62, -998},{ 309, -951},{ 535, -844},{ 728, -684},{ 876, -481},{ 968, -248},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 970, 239},{ 885, 464},{ 748, 663},{ 568, 822},{ 354, 935},{ 120, 992},{ -120, 992},{ -354, 935},{ -568, 822},{ -748, 663},{ -885, 464},{ -970, 239},{ -1000, 0},{ -970, -239},{ -885, -464},{ -748, -663},{ -568, -822},{ -354, -935},{ -120, -992},{ 120, -992},{ 354, -935},{ 568, -822},{ 748, -663},{ 885, -464},{ 970, -239},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 973, 230},{ 893, 448},{ 766, 642},{ 597, 802},{ 396, 918},{ 173, 984},{ -58, 998},{ -286, 957},{ -499, 866},{ -686, 727},{ -835, 549},{ -939, 342},{ -993, 116},{ -993, -116},{ -939, -342},{ -835, -549},{ -686, -727},{ -500, -866},{ -286, -957},{ -58, -998},{ 173, -984},{ 396, -918},{ 597, -802},{ 766, -642},{ 893, -448},{ 973, -230},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 974, 222},{ 900, 433},{ 781, 623},{ 623, 781},{ 433, 900},{ 222, 974},{ 0, 1000},{ -222, 974},{ -433, 900},{ -623, 781},{ -781, 623},{ -900, 433},{ -974, 222},{ -1000, 0},{ -974, -222},{ -900, -433},{ -781, -623},{ -623, -781},{ -433, -900},{ -222, -974},{ 0, -1000},{ 222, -974},{ 433, -900},{ 623, -781},{ 781, -623},{ 900, -433},{ 974, -222},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 976, 214},{ 907, 419},{ 796, 605},{ 647, 762},{ 468, 883},{ 267, 963},{ 54, 998},{ -161, 986},{ -370, 928},{ -561, 827},{ -725, 687},{ -856, 515},{ -947, 319},{ -994, 108},{ -994, -108},{ -947, -319},{ -856, -515},{ -725, -687},{ -561, -827},{ -370, -928},{ -161, -986},{ 54, -998},{ 267, -963},{ 468, -883},{ 647, -762},{ 796, -605},{ 907, -419},{ 976, -214},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 978, 207},{ 913, 406},{ 809, 587},{ 669, 743},{ 500, 866},{ 309, 951},{ 104, 994},{ -104, 994},{ -309, 951},{ -499, 866},{ -669, 743},{ -809, 587},{ -913, 406},{ -978, 207},{ -1000, 0},{ -978, -207},{ -913, -406},{ -809, -587},{ -669, -743},{ -500, -866},{ -309, -951},{ -104, -994},{ 104, -994},{ 309, -951},{ 500, -866},{ 669, -743},{ 809, -587},{ 913, -406},{ 978, -207},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 979, 201},{ 918, 394},{ 820, 571},{ 688, 724},{ 528, 848},{ 347, 937},{ 151, 988},{ -50, 998},{ -250, 968},{ -440, 897},{ -612, 790},{ -758, 651},{ -874, 485},{ -954, 299},{ -994, 101},{ -994, -101},{ -954, -299},{ -874, -485},{ -758, -651},{ -612, -790},{ -440, -897},{ -250, -968},{ -50, -998},{ 151, -988},{ 347, -937},{ 528, -848},{ 688, -724},{ 820, -571},{ 918, -394},{ 979, -201},{ 0, 0},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 980, 195},{ 923, 382},{ 831, 555},{ 707, 707},{ 555, 831},{ 382, 923},{ 195, 980},{ 0, 1000},{ -195, 980},{ -382, 923},{ -555, 831},{ -707, 707},{ -831, 555},{ -923, 382},{ -980, 195},{ -1000, 0},{ -980, -195},{ -923, -382},{ -831, -555},{ -707, -707},{ -555, -831},{ -382, -923},{ -195, -980},{ 0, -1000},{ 195, -980},{ 382, -923},{ 555, -831},{ 707, -707},{ 831, -555},{ 923, -382},{ 980, -195},{ 0, 0},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 981, 189},{ 928, 371},{ 841, 540},{ 723, 690},{ 580, 814},{ 415, 909},{ 235, 971},{ 47, 998},{ -142, 989},{ -327, 945},{ -499, 866},{ -654, 755},{ -786, 618},{ -888, 458},{ -959, 281},{ -995, 95},{ -995, -95},{ -959, -281},{ -888, -458},{ -786, -618},{ -654, -755},{ -500, -866},{ -327, -945},{ -142, -989},{ 47, -998},{ 235, -971},{ 415, -909},{ 580, -814},{ 723, -690},{ 841, -540},{ 928, -371},{ 981, -189},{ 0, 0},{ 0, 0}},
    {{ 1000, 0},{ 982, 183},{ 932, 361},{ 850, 526},{ 739, 673},{ 602, 798},{ 445, 895},{ 273, 961},{ 92, 995},{ -92, 995},{ -273, 961},{ -445, 895},{ -602, 798},{ -739, 673},{ -850, 526},{ -932, 361},{ -982, 183},{ -1000, 0},{ -982, -183},{ -932, -361},{ -850, -526},{ -739, -673},{ -602, -798},{ -445, -895},{ -273, -961},{ -92, -995},{ 92, -995},{ 273, -961},{ 445, -895},{ 602, -798},{ 739, -673},{ 850, -526},{ 932, -361},{ 982, -183},{ 0, 0}},
    {{ 1000, 0},{ 983, 178},{ 936, 351},{ 858, 512},{ 753, 657},{ 623, 781},{ 473, 880},{ 309, 951},{ 134, 990},{ -44, 998},{ -222, 974},{ -393, 919},{ -550, 834},{ -691, 722},{ -809, 587},{ -900, 433},{ -963, 266},{ -995, 89},{ -995, -89},{ -963, -266},{ -900, -433},{ -809, -587},{ -691, -722},{ -550, -834},{ -393, -919},{ -222, -974},{ -44, -998},{ 134, -990},{ 309, -951},{ 473, -880},{ 623, -781},{ 753, -657},{ 858, -512},{ 936, -351},{ 983, -178}}
};

static unsigned int prec = 1000;

static void drawTable(void);
static void drawPhilos(state_t * state, unsigned int n);

static int viewer_main(int argc, char ** argv);
static int equals(state_t * v1, state_t * v2, unsigned int size);
static int checkState(state_t * state, unsigned int size);
static void think();
static void eat();
static int take_sticks( philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem);
static int drop_sticks( philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem);
static void initializePhilos(philo_t * philos, unsigned int n_philos);
static void startPhilo(philo_t * philos, unsigned int pos);
static void addPhilo(philos_info_t * info, main_func_t * main_f);
static void removePhilo(philos_info_t * info);

/*
static void busyWait(uint32_t millis){
    unsigned int last_ticks = getTicks();
    printf("BUSY WAIT: %d\n",( millis * PIT_FREQUENCY ) / 1000 ); 

    while(getTicks() - last_ticks < (( millis * PIT_FREQUENCY ) / 1000)); 
}
*/

// How a philosopher thinks.
static void think() {
    
    sleep(GetUniform(2000));
}

// How a philosopher eats.
static void eat(state_t * state, sem_id state_lock, sem_id viewer_sem) {
    sleep(GetUniform(1000));
}

static int take_sticks(philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem) {

    sem_wait(state_lock);
    philo->state = HUNGRY;
    sem_post(state_lock); 
    sem_post(viewer_sem);

    if (philo->table_pos % 2 == 0) {
        //first take right stick
        sem_wait(right);  //sleep for it to be free
        philo->hands.right = right; 
        //printf("Philo %d take right stick (stick sem_id n: %d)\n", philo->table_pos, right);

        sleep(GetUniform(1000)); 
        //then take left stick

        sem_wait(left);

        sem_wait(state_lock);
        philo->state = EATING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        philo->hands.left = left;
        //printf("Philo %d take left stick (stick sem_id n: %d)\n", philo->table_pos, left);
        
    }else{
        //first take left stick 
        sem_wait(left);
        philo->hands.left = left; 
        //printf("Philo %d take left stick (stick sem_id n: %d)\n", philo->table_pos, left);

        sleep(GetUniform(1000));

        //then take right stick 
        sem_wait(right);  //sleep for it to be free

        sem_wait(state_lock);
        philo->state = EATING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        philo->hands.right = right; 
        //printf("Philo %d take right stick (stick sem_id n: %d)\n", philo->table_pos, right);
    }

    return 0;    
}
 
/* 
static int take(int idx, sem_id * sticks, sem_id * hand){

    if(sem_wait(sticks[idx]) == -1) {
        return -1;
    } 
    *hand = idx; 

    return 0;
}
*/
 
static int drop_sticks(philo_t * philo, sem_id left, sem_id right, sem_id state_lock, sem_id viewer_sem){
    
    if (philo->table_pos % 2 == 0) {
        philo->hands.left = -1;

        sem_wait(state_lock);

        sem_post(left);

        philo->state = THINKING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        //printf("Philo %d drop left stick (stick sem_id n: %d)\n", philo->table_pos, left);

        sleep(GetUniform(1000));

        philo->hands.right = -1;
        sem_post(right);
        //printf("Philo %d drop right stick (stick sem_id n: %d)\n", philo->table_pos, right);
    }
    else {
        philo->hands.right = -1;

        sem_wait(state_lock);

        sem_post(right);

        philo->state = THINKING;
        sem_post(state_lock);
        sem_post(viewer_sem);

        //printf("Philo %d drop right stick (stick sem_id n: %d)\n", philo->table_pos, right);

        sleep(GetUniform(1000));

        philo->hands.left = -1;
        sem_post(left);
        //printf("Philo %d drop left stick (stick sem_id n: %d)\n", philo->table_pos, left);
    }   

    return 0;
}

static void addPhilo(philos_info_t * info, main_func_t * main_f){
    unsigned int i = info->n_philos;

    startPhilo(info->philos, i);
    char philo_name[PHILO_NAME_MAX_LENGTH];
    (info->args)[i] = malloc(4 * sizeof(char *));
    (info->args)[i][0] = malloc(MAX_ARG_LENGTH * sizeof(char));
    (info->args)[i][1] = malloc(MAX_ARG_LENGTH * sizeof(char));
    (info->args)[i][2] = malloc(MAX_ARG_LENGTH * sizeof(char));
    (info->args)[i][3] = NULL;
    sprintf(philo_name, "Philo %d", i);
    sprintf((info->args)[i][0], "%p", &((info->philos)[i]));
    sprintf((info->args)[i][1], "%p", info->sticks);
    sprintf((info->args)[i][2], "%p", &(info->n_philos));
    main_f->argv = (info->args)[i];
    
    char sem_name[SEM_NAME_MAX_LENGTH];
    sprintf(sem_name, "Stick %d", i);
    (info->sticks)[i] = sem_open(sem_name);

    (info->n_philos)++;
    createProcess(main_f, philo_name, 0, NULL, NULL, &((info->philos)[i].pid));
}

static void removePhilo(philos_info_t * info){
    sem_id left, right;
    unsigned int i = info->n_philos - 1;

    left = (info->sticks)[i];
    right = (info->sticks)[0];

    if (i % 2 == 0) {
        sem_wait(right);
        sem_wait(left);
    }else{
        sem_wait(left);
        sem_wait(right);
    }

    kill((info->philos)[i].pid);

    if (i % 2 == 0) {
        sem_post(left);
        sem_post(right);
    }else{
        sem_post(right);
        sem_post(left);
    }

    sem_close(left); 

    free((info->args)[i][0]);
    free((info->args)[i][1]);
    free((info->args)[i][2]);
    free((info->args)[i]);

    (info->n_philos)--;
}

static int philo_main(int argc, char ** argv) {

    philo_t * me;
    sem_id * sticks;
    unsigned int * n_philos;
    unsigned int n;

    sem_id left_stick_sem, right_stick_sem;

    char left_stick_name[SEM_NAME_MAX_LENGTH];
    char right_stick_name[SEM_NAME_MAX_LENGTH];

    sem_id state_lock = sem_open("state_lock");
    sem_id viewer_sem = sem_open("philo_viewer");

    sscanf(argv[0], "%p", &me);
    sscanf(argv[1], "%p", &sticks);
    sscanf(argv[2], "%p", &n_philos);

    while(1){

        n = *n_philos;
        //printf("Philo %d is hungry\n", me->table_pos);
        sprintf(left_stick_name, "Stick %d", (me->table_pos) % n);
        sprintf(right_stick_name, "Stick %d", ((me->table_pos) + 1) % n);

        left_stick_sem = sem_open(left_stick_name);
        right_stick_sem = sem_open(right_stick_name);

        take_sticks(me, left_stick_sem, right_stick_sem, state_lock, viewer_sem);
        //printf("Philo %d is going to eat\n", me->table_pos);
        eat(&(me->state), state_lock, viewer_sem);
        drop_sticks(me, left_stick_sem, right_stick_sem, state_lock, viewer_sem);
        //printf("Philo %d is going to think\n", me->table_pos); 
        think(&(me->state), state_lock, viewer_sem);
        sem_close(left_stick_sem);
        sem_close(right_stick_sem);
    }

    return 0;
}

int thinking_philos_main(int argc, char ** argv) {

    philos_info_t info;
    int c;
    info.n_philos = INITIAL_PHILOS;

    initializePhilos(info.philos, info.n_philos);
    
    for (unsigned int i = 0; i < info.n_philos; i++) {
        (info.args)[i] = malloc(3 * sizeof(char *));
        (info.args)[i][0] = malloc(MAX_ARG_LENGTH * sizeof(char));
        (info.args)[i][1] = malloc(MAX_ARG_LENGTH * sizeof(char));
        (info.args)[i][2] = malloc(MAX_ARG_LENGTH * sizeof(char));
        (info.args)[i][3] = NULL;
    }

    main_func_t f_aux = {philo_main, 2, NULL};

    sem_id state_lock = sem_open("state_lock");
    sem_id viewer_sem = sem_open("philo_viewer");
    uint64_t viewer_pid;

    char ** viewer_args = malloc(2 * sizeof(char *));
    viewer_args[0] = malloc(MAX_ARG_LENGTH * sizeof(char));

    main_func_t f_viewer = {viewer_main, 1, viewer_args};

    sprintf(viewer_args[0], "%p", &info);

    for (unsigned int i = 0; i < info.n_philos; i++) {
        char sem_name[SEM_NAME_MAX_LENGTH];
        sprintf(sem_name, "Stick %d", i);
        info.sticks[i] = sem_open(sem_name);
    }

    char philo_name[PHILO_NAME_MAX_LENGTH];

    for (unsigned int i = 0; i < info.n_philos; i++) {
        sprintf(philo_name, "Philo %d", i);
        sprintf((info.args)[i][0], "%p", &(info.philos[i]));
        sprintf((info.args)[i][1], "%p", info.sticks);
        sprintf((info.args)[i][2], "%p", &(info.n_philos));
        f_aux.argv = (info.args)[i];
        createProcess(&f_aux, philo_name, 0, NULL, NULL, &info.philos[i].pid);
    }

    createProcess(&f_viewer, "Philo Viewer", 0, NULL, NULL, &viewer_pid);

    clearScreen();

    while ((c = scanChar()) != ESC) {
        if (c == 'a' && info.n_philos < MAX_PHILOS)
            addPhilo(&info, &f_aux);
        else if (c == 'r' && info.n_philos > MIN_PHILOS)
            removePhilo(&info);
        else if (c == 'x')
            for(unsigned int i = 0; i < info.n_philos; i++)
                kill(info.philos[i].pid);
        else if (c == 'p')
            printProcesses();
    }
    //finish process 

    kill(viewer_pid);

    free(viewer_args[0]);
    free(viewer_args);

    for (unsigned int i = 0; i < info.n_philos; i++)
        kill(info.philos[i].pid);

    for (unsigned int i = 0; i < info.n_philos; i++) {
        free((info.args)[i][0]);
        free((info.args)[i][1]);
        free((info.args)[i][2]);
        free((info.args)[i]);
    }

    for (unsigned int i = 0; i < info.n_philos; i++)
        sem_destroy(info.sticks[i]);

    sem_destroy(viewer_sem);
    sem_destroy(state_lock);

    clearScreen();

    return 0;
}

static void initializePhilos(philo_t * philos, unsigned int n_philos) {
    for(unsigned int i = 0; i < n_philos; i++)
        startPhilo(philos, i);
}

static void startPhilo(philo_t * philos, unsigned int pos) {
    philos[pos].hands.left = -1;
    philos[pos].hands.right = -1;
    philos[pos].pid = -1;
    philos[pos].state = HUNGRY;
    philos[pos].table_pos = pos;
}

static int viewer_main(int argc, char ** argv) {
    philos_info_t * info;
    //int sval;
    sscanf(argv[0], "%p", &info);

    unsigned int i;
    unsigned int n;
    state_t states[2][MAX_PHILOS] = {{0}, {0}};
    state_t actual_state = 0;
    state_t * last_state;
    state_t * state;

    sem_id viewer_sem = sem_init_open("philo_viewer", 0);
    sem_id state_lock = sem_open("state_lock");

    drawTable();
     
    while(1) {
        sem_wait(viewer_sem); //in case a philo is changing state

        n = info->n_philos;
        last_state = states[actual_state];
        state = states[1 - actual_state];

        sem_wait(state_lock);

        for(i = 0; i < info->n_philos; i++)
            state[i] = (info->philos)[i].state;
        
        sem_post(state_lock);

        if (!equals(state, last_state, n) == 0) {
            if (checkState(state, n) != 0)
                printf("An error ocurred. Two processes eating together!\n");
            clearScreen();
            drawTable();
            drawPhilos(state, n);
            actual_state = 1 - actual_state;
        }

        /*
        if (!equals(state, last_state, n) == 0) {
            if (checkState(state, n) != 0)
                printf("An error ocurred. Two processes eating together!\n");
            else {
                for(i = 0; i < info->n_philos; i++)
                    printf("%s ", (state[i] == EATING)?"E":".");
                printf("\n");
            }
            actual_state = 1 - actual_state;
        }
        */
    }

    return 0;
    
}

static int equals(state_t * v1, state_t * v2, unsigned int size) {
    for (int i = 0; i < size; i++)
        if (v1[i] - v2[i] != 0)
            return v1[i] - v2[i];
    return 0;
}

static int checkState(state_t * state, unsigned int size) {
    for (int i = 0; i < size; i++)
        if (state[i] == EATING && state[(i+1)%size] == EATING)
            return -1;
    return 0;
}

static void drawTable(void) {
    drawCircle(TABLE_X, TABLE_Y, TABLE_RADIUS, TABLE_COLOR);
}

static void drawPhilos(state_t * state, unsigned int n) {
    int philo_x, philo_y;
    unsigned int philo_dist;
    unsigned int divider = (n <= 2)?10:((n <= 3)?8:6);
    unsigned int philo_radius = TABLE_PERIMETER / (divider * n);
    unsigned int perc = (n <= 2)?200:150;
    uint32_t philo_color;
    for (unsigned int i = 0; i < n; i++) {
        philo_dist = (state[i] == EATING)?TABLE_RADIUS:((TABLE_RADIUS * perc) / 100);

        if (drawing_positions[n-1][i][0] < 0)
            philo_x = TABLE_X - ((philo_dist * (-1 * (drawing_positions[n-1][i][0]) ) ) / prec);
        else
            philo_x = TABLE_X + ((philo_dist * drawing_positions[n-1][i][0]) / prec);

        if (drawing_positions[n-1][i][1] < 0)
            philo_y = TABLE_Y - ((philo_dist * (-1 * (drawing_positions[n-1][i][1]) ) ) / prec);
        else
            philo_y = TABLE_Y + ((philo_dist * drawing_positions[n-1][i][1]) / prec);
        philo_color = (state[i] == EATING)?(EATING_PHILO_COLOR):((state[i] == THINKING)?(THINKING_PHILO_COLOR):(HUNGRY_PHILO_COLOR));
        drawCircle(philo_x, philo_y, philo_radius, philo_color);
    }
}