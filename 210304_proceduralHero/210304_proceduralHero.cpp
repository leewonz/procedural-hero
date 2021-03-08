// 210304_proceduralHero.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
/*
    [영웅은 절차적]

    1. 텍스트로 진행되는 게임
    2. 게임이 시작되면 영웅의 이름과 난이도를 입력
    3. 난이도에 따라서 영웅의 HP, 만나는 몬스터의 숫자가 변동
    4. 몬스터를 잡으면 랜덤하게 돈을 획득한다. 0~100
    5. 몬스터를 잡으면 던전을 계속 탐험할 지 상점을 들를 지 결정한다.
    6. 상점에 들르면 돈을 소모해서 HP를 회복할 수 있다.
    - 6.1 전체 회복, +10, +20, +30 차등 회복
    7. HP가 0이 되면 게임 오버
    8. 몬스터를 잡으면 게임 클리어

    0. 전투는 가위바위보 
    - 0.1 비기면 승패가 정해질 때까지 반복
    - 0.2 졌을 때만 영웅 HP가 줄어든다.
    - 0.3 몬스터는 한번 지면 바로 죽음
*/

#include <iostream>
#include <ctime>
#include <conio.h>

using namespace std;

int main()
{

    const int MAP_W = 16;
    const int MAP_H = 16;
    const int mapEnemyCount_max[3] = { 30, 45, 60 };
    const int mapShopCount_max[3] = { 8, 6, 4 };
    const int mapKeyCount_max[3] = { 1, 2, 3 };
    const int mapGoalCount_max = 1;

    const string ENEMY_NAME[3] = { "박쥐", "고블린", "악마" };
    const int ENEMY_ATK_VAL[3] = { 15, 20, 25 };

    bool isProgramEnded = false;

    while (!isProgramEnded) // Whole Loop
    {
        char map[MAP_H][MAP_W];

        int randomPos;
        int mapEnemyCount = 0;
        int mapShopCount = 0;
        int mapGoalCount = 0;
        int mapKeyCount = 0;

        int enemyMoneyDrop_max[3] = { 90, 70, 50 };
        int enemyMoneyDrop_min[3] = { 20, 15, 10 };

        int goalPosX = 0;
        int goalPosY = 0;

        string plName;
        int plPosX = 0;
        int plPosY = 0;
        int plMaxHP = 100;
        int plHP = plMaxHP;
        int plMoney = 20;

        int step = 0;

        bool isInMap = true;
        bool isInShop = false;
        bool isInBattle = false;
        bool isInGame = true;
        bool isWin = false;
        bool isLost = false;
        bool isPlAtGoal = false;
        bool isPlAtKey = false;
        
        bool isGameInit = true;
        bool isMapInit = true;

        int difficulty = -1;

        srand(time(NULL));

        cout << "      ~~~~~~~~~~~~~~~~~~~~~~~" << endl
            << "      ~    용사는 절차적    ~" << endl
            << "      ~~~~~~~~~~~~~~~~~~~~~~~" << endl
            << endl
            << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl
            << "1. 쉬움" << endl
            << "2. 보통" << endl
            << "3. 어려움" << endl
            << "0. 종료" << endl
            << "번호를 입력하세요 : ";

        do {
            int difficultyInput = _getch();
            switch (difficultyInput)
            {
            case '1':
                difficulty = 0;
                cout << "1. 쉬움" << endl;
                break;
            case '2':
                difficulty = 1;
                cout << "2. 보통" << endl;
                break;
            case '3':
                difficulty = 2;
                cout << "3. 어려움" << endl;
                break;
            case '0':
                isProgramEnded = true;
                cout << "0. 종료" << endl;
                break;
            default:
                break;
            }

        } while (difficulty == -1 && !isProgramEnded);

        if (!isProgramEnded) {

            cout << "용사의 이름을 입력하세요 : ";
            cin >> plName;
            plName = plName == "" ? "용사" : plName;

            // 맵 초기화
            for (int i = 0; i < MAP_H; i++) {
                for (int j = 0; j < MAP_W; j++) {
                    map[i][j] = ' ';
                }
            }


            // 적 배치하기
            while (mapEnemyCount < mapEnemyCount_max[difficulty]) {
                randomPos = rand() % (MAP_H * MAP_W - 1) + 1;
                int randomPosY = randomPos / MAP_H;
                int randomPosX = randomPos % MAP_W;
                if (map[randomPosY][randomPosX] == ' ') {
                    mapEnemyCount++;
                    int monsterType = rand() % 3;
                    if (monsterType == 0) {
                        map[randomPosY][randomPosX] = 'B'; //Bat (0)
                    }
                    else if (monsterType == 1) {
                        map[randomPosY][randomPosX] = 'G'; //Goblin (1)
                    }
                    else {
                        map[randomPosY][randomPosX] = 'D'; //Demon (2)
                    }
                }
            }

            // 상점 배치하기
            while (mapShopCount < mapShopCount_max[difficulty]) {
                randomPos = rand() % (MAP_H * MAP_W - 1) + 1;
                int randomPosY = randomPos / MAP_H;
                int randomPosX = randomPos % MAP_W;
                if (map[randomPosY][randomPosX] == ' ') {
                    mapShopCount++;
                    map[randomPosY][randomPosX] = 'S'; //Shop
                }
            }

            // 열쇠 배치하기
            while (mapKeyCount < mapKeyCount_max[difficulty]) {
                randomPos = rand() % (MAP_H * MAP_W - 1) + 1;
                int randomPosY = randomPos / MAP_H;
                int randomPosX = randomPos % MAP_W;
                if (map[randomPosY][randomPosX] == ' ') {
                    mapKeyCount++;
                    map[randomPosY][randomPosX] = 'K'; //Key
                }
            }

            // 탈출구 배치하기
            while (mapGoalCount < mapGoalCount_max) {
                goalPosY = rand() % (MAP_H / 2) + MAP_H - (MAP_H / 2);
                goalPosX = rand() % (MAP_W / 2) + MAP_W - (MAP_W / 2);
                if (map[goalPosY][goalPosX] == ' ') {
                    mapGoalCount++;
                    map[goalPosY][goalPosX] = 'E'; //Exit
                }
            }

            //Main Game Loop
            //cout << "메인 루프 시작" << endl;
            while (isInGame) {

                int battleMonsterType = 0;

                if (isGameInit) {
                    isGameInit = false;
                }

                if (isMapInit) {
                    system("CLS");
                    isMapInit = false;
                }
                else {
                    // 1. 플레이어 입력 대기
                    // 2. 스크린 초기화
                    // 3. 맵에 플레이어 위치 없애기
                    // 4. 플레이어 위치 이동
                    // 5. 플레이어 위치에 따른 이벤트 상태 ON
                    //    - 전투, 상점, 승리
                    // 6. 맵에 플레이어 위치 나타내기
                    // 7. 맵 표시
                    // 8. 돈, 체력 표시
                    // 9. (전투 이벤트가 발생했다면) 전투 루프 진입
                    //10. (상점 이벤트가 발생했다면) 상점 루프 진입
                    //11. (열쇠 이벤트가 발생했다면) 열쇠 획득
                    //12. (승리 이벤트가 발생했다면) 승리

                    // 1. 플레이어 입력 대기

                    int mapUserInput = _getch();
                    if (mapUserInput >= 'a' && mapUserInput <= 'z') {
                        mapUserInput += 'A' - 'a';
                    }

                    // 2. 스크린 초기화
                    system("CLS");

                    // 3. 맵에 플레이어 위치 없애기
                    if (plPosX == goalPosX && plPosY == goalPosY) {
                        map[plPosY][plPosX] = 'E';
                    }
                    else {
                        map[plPosY][plPosX] = '.';
                    }

                    // 4. 플레이어 위치 이동
                    switch (mapUserInput) {
                    case 'W':
                        if (plPosY != 0) { plPosY--; step++; }
                        break;
                    case 'S':
                        if (plPosY != MAP_H - 1) { plPosY++; step++; }
                        break;
                    case 'A':
                        if (plPosX != 0) { plPosX--; step++; }
                        break;
                    case 'D':
                        if (plPosX != MAP_W - 1) { plPosX++; step++; }
                        break;
                    }

                    // 5. 플레이어 위치에 따른 이벤트 상태 ON
                    switch (map[plPosY][plPosX]) {
                    case 'B': {
                        isInMap = false;
                        isInBattle = true;
                        battleMonsterType = 0;
                        break;
                    }
                    case 'G': {
                        isInMap = false;
                        isInBattle = true;
                        battleMonsterType = 1;
                        break;
                    }
                    case 'D': {
                        isInMap = false;
                        isInBattle = true;
                        battleMonsterType = 2;
                        break;
                    }
                    case 'S': {
                        isInMap = false;
                        isInShop = true;
                        break;
                    }
                    case 'E': {
                        isPlAtGoal = true;
                        break;
                    }
                    case 'K': {
                        mapKeyCount--;
                        isPlAtKey = true;
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                }

                // 6. 맵에 플레이어 위치 나타내기
                map[plPosY][plPosX] = '@';

                // 7, 8. 맵 표시, 스탯 표시
                cout << plName << "는 절차적              난이도 : ";
                switch (difficulty)
                {
                case 0:
                    cout << "쉬움";
                    break;
                case 1:
                    cout << "보통";
                    break;
                case 2:
                    cout << "어려움";
                    break;
                default:
                    break;
                }
                cout << endl << "┏";
                for (int j = 0; j < MAP_W; j++) {
                    cout << "━";
                }
                cout << "┓";
                cout << endl;
                for (int i = 0; i < MAP_H; i++) {
                    cout << "┃";
                    for (int j = 0; j < MAP_W; j++) {
                        if (map[i][j] == 'B' || map[i][j] == 'G' || map[i][j] == 'D') {
                            cout << ' ';
                        }
                        else {
                            cout << map[i][j];
                        }
                    }
                    cout << "┃";

                    switch (i)
                    {
                    case 1:
                        cout << "   " << plName << "의 체력 : " << plHP;
                        break;
                    case 2:
                        cout << "   " << plName << "의 돈 : " << plMoney;
                        break;
                    case 4:
                        cout << "   남은 적의 수 : " << mapEnemyCount;
                        break;
                    case 5:
                        cout << "   남은 열쇠의 수 : " << mapKeyCount;
                        break;
                    case 6:
                        cout << "   걸음 수 : " << step;
                        break;
                    case 8:
                        cout << "   K - 열쇠 ";
                        break;
                    case 9:
                        cout << "   S - 상점 ";
                        break;
                    case 10:
                        cout << "   E - 탈출구 ";
                        break;
                    default:
                        break;
                    }
                    cout << endl;

                }
                cout << "┗";
                for (int j = 0; j < MAP_W; j++) {
                    cout << "━";
                }
                cout << "┛";
                cout << endl;


                // 9. 전투 이벤트 진입
                if (isInBattle) {

                    system("CLS");

                    //가위바위보 전투
                    cout << ENEMY_NAME[battleMonsterType] << "이 나타났다." << endl
                        << ENEMY_NAME[battleMonsterType] << "의 공격력 : " << ENEMY_ATK_VAL[battleMonsterType] << endl
                        << plName << "의 체력 : " << plHP << endl
                        << "전투 시작!" << endl;
                    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;

                    int enemyAttackVal = ENEMY_ATK_VAL[battleMonsterType];

                    string RSP[3] = { "가위", "바위", "보" };

                    bool winFight = false;
                    bool loseFight = false;


                    while (!winFight && !loseFight)
                    {
                        int enemyChoice = rand() % 3;
                        string plInput;
                        int plChoice = -1;
                        int rspResult;

                        while (plChoice == -1) {
                            cout << "가위, 바위, 보 중 하나를 입력하세요 : ";
                            cin >> plInput;

                            for (int i = 0; i < 3; i++) {
                                if (plInput == RSP[i]) {
                                    plChoice = i;
                                }
                            }
                        }

                        cout << "당신의 입력: " << RSP[plChoice] << endl;
                        cout << ENEMY_NAME[battleMonsterType] << "의 입력: " << RSP[enemyChoice] << endl;

                        //0: 비김 1: 이김 2: 짐 
                        rspResult = plChoice - enemyChoice;
                        rspResult += (plChoice - enemyChoice < 0) ? 3 : 0;

                        switch (rspResult) {
                        case 0: { // 비김
                            cout << "무승부!" << endl;
                            cout << plName << "의 체력 : " << plHP << endl;

                            break;
                        } case 1: { // 이김
                            cout << plName << "의 공격!" << endl
                                << ENEMY_NAME[battleMonsterType] << "을 쓰러뜨렸다!" << endl;

                            winFight = true;

                            break;
                        } case 2: { // 짐
                            cout << ENEMY_NAME[battleMonsterType] << "의 공격!" << endl
                                << plName << "는 " << enemyAttackVal << " 피해를 입었다!" << endl;
                            plHP = (plHP - enemyAttackVal >= 0) ? plHP - enemyAttackVal : 0;
                            cout << plName << "의 체력 : " << plHP << endl;

                            if (plHP == 0) {
                                loseFight = true;
                            }

                            break;
                        } default: {
                            cout << "[!!ERROR!! 잘못된 승부 결과입니다]" << endl;
                            break;
                        }
                        }
                        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
                    }
                    if (winFight) {
                        int moneyGain = rand() %
                            (enemyMoneyDrop_max[difficulty] + 1 - enemyMoneyDrop_min[difficulty]) +
                            enemyMoneyDrop_min[difficulty];

                        isInBattle = false;
                        isMapInit = true;
                        isInMap = true;
                        cout << "전투에서 승리했다! 돈을 " << moneyGain << " 획득했다!" << endl;
                        cout << plName << "의 체력 : " << plHP << endl;
                        cout << plName << "의 돈 : " << plMoney << " + " << moneyGain << endl;
                        plMoney += moneyGain;
                        mapEnemyCount--;
                    }
                    else if (loseFight) {
                        isInBattle = false;
                        isInMap = true;
                        isInGame = false;
                        cout << plName << "가 쓰러졌다..." << endl;
                        cout << "패배!" << endl;
                    }
                    cout << "****************[확인]****************";
                    _getch();
                    isMapInit = true;
                }

                //10. (상점 이벤트가 발생했다면) 상점 루프 진입
                if (isInShop) {

                    int plShopInput;
                    int selectNum = -1;

                    do {

                        bool stock[] = { true, true, true };
                        int price[] = { 50, 100, 150 };
                        int healAmt[] = { 20, 50, 999999 };

                        int healHP = 0;

                        bool isShopHPFull = false;
                        bool isShopNoMoney = false;

                        selectNum = -1;

                        system("CLS");

                        cout << "     ~$$$$$▒ 상 - 점 ▒$$$$$~     " << endl
                            << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl
                            << "1. 소형 포션 (HP + 20)   비용: 50    " << endl
                            << "2. 중형 포션 (HP + 50)   비용: 100    " << endl
                            << "3. 대형 포션 (HP + FULL) 비용: 150   " << endl
                            << "0. 나가기" << endl
                            << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
                        cout << plName << "의 체력 : " << plHP << endl;
                        cout << plName << "의 돈 : " << plMoney << endl;
                        cout << "원하는 상품 번호를 입력하세요 : ";

                        plShopInput = _getch();

                        switch (plShopInput)
                        {
                        case '1': {
                            cout << '1' << endl;
                            selectNum = 1;
                            break;
                        case '2':
                            cout << '2' << endl;
                            selectNum = 2;
                            break;
                        case '3':
                            cout << '3' << endl;
                            selectNum = 3;
                            break;
                        case '0':
                            cout << '0' << endl;
                            selectNum = 0;
                            break;
                        }
                        default:
                            selectNum = -1;
                            break;
                        }

                        if (selectNum != -1
                            && stock[selectNum - 1]
                            && plMoney >= price[selectNum - 1]
                            && plHP < plMaxHP) {

                            healHP = healAmt[selectNum - 1];
                        }
                        else if (plMoney < price[selectNum + -1]) {
                            isShopNoMoney = true;
                        }
                        else if (plHP == plMaxHP) {
                            isShopHPFull = true;
                        }

                        if (selectNum != -1) {
                            if (healHP != 0 && selectNum >= 1) {
                                plMoney -= price[selectNum - 1];
                                healHP = (plHP + healHP < plMaxHP) ? healHP : plMaxHP - plHP;
                                plHP += healHP;
                                cout << "체력이 " << healHP << "만큼 회복되었다." << endl;
                            }
                            else if (selectNum == 0) {
                                cout << "상점에서 나간다." << endl;
                            }
                            else if (isShopNoMoney) {
                                cout << "돈이 없다!" << endl;
                            }
                            else if (isShopHPFull) {
                                cout << "이미 체력이 가득이다!" << endl;
                            }

                            cout << "****************[확인]****************";
                            _getch();
                        }



                    } while (selectNum != 0);

                    isInShop = false;
                    isMapInit = true;
                    isInMap = true;

                }

                //11. 열쇠 획득 이벤트
                if (isPlAtKey) {
                    isPlAtKey = false;
                    if (mapKeyCount == 0) {
                        cout << "열쇠를 전부 획득했다!" << endl
                            << "탈출구로 이동하자!" << endl;
                    }
                    else {
                        cout << "열쇠를 획득했다!" << endl
                            << "탈출까지 앞으로 " << mapKeyCount << "개 남았다." << endl;
                    }
                    cout << "****************[확인]****************";
                    _getch();
                    isMapInit = true;
                }

                //12. (승리 이벤트가 발생했다면) 승리
                if (isPlAtGoal) {
                    isPlAtGoal = false;
                    if (mapKeyCount <= 0) {
                        cout << plName << "는 탈출에 성공했습니다. 승리!" << endl;
                        isWin = true;
                        isInGame = false;
                    }
                    else {
                        cout << "열쇠를 전부 모아야 탈출할 수 있습니다..." << endl;
                    }
                    cout << "****************[확인]****************";
                    _getch();
                    isMapInit = true;
                }

            }//END Main Game Loop (restart)
            system("CLS");

        }

    }//END Whole Loop
}
