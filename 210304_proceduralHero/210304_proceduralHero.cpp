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

    
    -난이도에 따라 맵의 크기가 바뀐다.
    -영웅이 2칸 움직일 때마다 몬스터 1칸 움직인다.

    - 배열, 구조체 함수를 최대한 활용.
    - 맵의 구성은 타일로 구성된다.
    - 타일의 종류: 숲, 늪, 땅
    - 타일의 종류마다 만나는 몬스터가 다르다.
    - 타일: x좌표, y좌표, 타입, 모양
    - 플레이어: 이름, 최대HP (레벨에 따라 증가), 경험치, 레벨, 소유 골드
    - 몬스터: 이름, 최대HP, 현재HP, 획득 경험치, 획득 골드
    - 물약: 이름, 가격, 회복량
    - 상점: 물약의 종류와 수량
*/

#include <iostream>
#include <ctime>
#include <conio.h>

using namespace std;


const int NUM_DIFFICULTY_TYPE = 3;
const int NUM_ENEMY_TYPE = 3;
const int NUM_TILE_TYPE = 3;
const int NUM_POTION_TYPE = 3;


struct Player {
    int x;
    int y;
    int hp;
    int hp_max;
    int hp_max_inc;
    int money;
    int exp;
    int exp_max;
    int exp_max_inc;
    int level;

    string name;
};

struct DifficultyType {
    int map_w;
    int map_h;
    float moneyGainCoeff;
    int mapEnemyCount_init;
    int mapShopCount_init;
    int mapKeyCount_init;
};

struct tileType {
    string name;
    char symbol;
};

struct Tile {
    int x;
    int y;
    int type;
};

struct EnemyType {
    string name;
    int hp;
    int attackValue;
    int expGain;
    int moneyGain_min;
    int moneyGain_max;
    int tileType;
    char symbol;
};

struct Enemy {
    int x;
    int y;
    int type;
    bool isAlive;
};

struct PotionType {
    string name;
    int price;
    int healAmt;
};

struct Shop {
    int numPotions[NUM_POTION_TYPE];
};

struct Key {
    int x;
    int y;
};

struct Goal {
    int x;
    int y;
};

struct MapInfo {
    int mapH;
    int mapW;
    int enemyCount;
    int enemyCount_init;
    int shopCount;
    int shopCount_init;
    int keyCount;
    int keyCount_init;
};

struct Collision {
    bool isPlayer;
    bool isEnemy;
    bool isShop;
    bool isKey;
    bool isExit;
    int enemyType;
    int instIdx;
};


const tileType TILE_TYPES[NUM_TILE_TYPE] =
{
    {"숲", '`'},
    {"늪", '~'},
    {"땅", ' '}
};

const EnemyType ENEMY_TYPES[NUM_ENEMY_TYPE] =
{
    {"위습", 30, 15, 20, 30, 60, 0, 'W'},
    {"멀록", 40, 20, 30, 40, 80, 1, 'M'},
    {"고블린", 50, 25, 40, 50, 100, 2, 'G'}
};

const DifficultyType DIFFICULTY_TYPES[NUM_DIFFICULTY_TYPE] =
{
    { 10, 10, 1.0, 20, 7, 1 },
    { 12, 12, 0.8, 40, 6, 2 },
    { 14, 14, 0.6, 60, 5, 3 }
};

const PotionType POTION_TYPES[NUM_POTION_TYPE] =
{
    {"소형 포션", 50, 20},
    {"중형 포션", 100, 50},
    {"대형 포션", 150, 200}
};

Player CreatePlayer(string name);

Tile** CreateTiles(int height, int width);

char** CreateEmptyMap(int height, int width);

Collision CheckMapChar(int x, int y, char** map);

void MoveAllEnemy(char** map, Tile** tiles, Enemy enemyList[], MapInfo mapInfo);

bool PlayerMove(Player& pl, MapInfo mapInfo, int input);

void PrintMap(char** map, Tile** tiles, Player pl, MapInfo mapInfo, int difficulty, int step);

MapInfo MapInitChar(
    char** map,
    Enemy* enemyList,
    Shop* shopList,
    Key* keyList,
    Goal& goal,
    int difficulty);

void VisitShop(Player& pl);

bool FightEnemy(Player& pl, int battleMonsterType, int difficulty);

int main()
{
    int mapEnemyCount_init;
    int mapShopCount_init;
    int mapKeyCount_init;
    int mapGoalCount_init = 1;

    bool isProgramEnded = false;


    while (!isProgramEnded) // Whole Loop
    {
        char** map;
        Tile** tiles;

        int randomPos;
        int mapEnemyCount = 0;
        int mapShopCount = 0;
        int mapGoalCount = 0;
        int mapKeyCount = 0;

        int enemyMoneyDropMax;
        int enemyMoneyDropMin;

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


        Enemy* enemyList;
        Shop* shopList;
        Key* keyList;
        Goal goal;

        Player pl;



        /*
            const int mapEnemyCount_max[3] = { 30, 45, 60 };
            const int mapShopCount_max[3] = { 8, 6, 4 };
            const int mapKeyCount_max[3] = { 1, 2, 3 };
        */

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

        MapInfo mapInfo;

        mapInfo.mapH = DIFFICULTY_TYPES[difficulty].map_h;
        mapInfo.mapW = DIFFICULTY_TYPES[difficulty].map_w;
        mapInfo.enemyCount_init = DIFFICULTY_TYPES[difficulty].mapEnemyCount_init;
        mapInfo.shopCount_init = DIFFICULTY_TYPES[difficulty].mapShopCount_init;
        mapInfo.keyCount_init = DIFFICULTY_TYPES[difficulty].mapKeyCount_init;

        mapInfo.enemyCount = mapInfo.enemyCount_init;
        mapInfo.shopCount = mapInfo.shopCount_init;
        mapInfo.keyCount = mapInfo.keyCount_init;

        enemyList = new Enemy[mapInfo.enemyCount_init];
        shopList = new Shop[mapInfo.shopCount_init];
        keyList = new Key[mapInfo.keyCount_init];

        if (!isProgramEnded) {

            cout << "용사의 이름을 입력하세요 : ";
            cin >> plName;
            plName = plName == "" ? "용사" : plName;

            pl = CreatePlayer(plName);

            // 타일 초기화
            tiles = CreateTiles(mapInfo.mapH, mapInfo.mapW);

            // 맵 초기화
            map = CreateEmptyMap(mapInfo.mapH, mapInfo.mapW);

            MapInitChar(
                map, enemyList, shopList, keyList, goal, difficulty);

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
                    if (pl.x == goal.x && pl.y == goal.y) {
                        map[pl.y][pl.x] = 'E';
                    } else {
                        map[pl.y][pl.x] = ' ';
                    }

                    // 4. 플레이어 위치 이동
                    bool isPlayerMoved = PlayerMove(pl, mapInfo, mapUserInput);
                    if (isPlayerMoved) { step++; }

                    // 몬스터 움직임
                    if ((step % 2 == 0) && isPlayerMoved) {
                        MoveAllEnemy(map, tiles, enemyList, mapInfo);
                    }

                    // 5. 플레이어 위치에 따른 이벤트 상태 ON
                    Collision pl_col = CheckMapChar(pl.x, pl.y, map);
                    
                    if (pl_col.isEnemy) {
                        isInMap = false;
                        isInBattle = true;
                        battleMonsterType = pl_col.enemyType;
                    } else if (pl_col.isShop) {
                        isInMap = false;
                        isInShop = true;
                    } else if (pl_col.isKey) {
                        mapInfo.keyCount--;
                        isPlAtKey = true;
                    } else if (pl_col.isExit) {
                        isPlAtGoal = true;
                    }
                }

                // 6. 맵에 플레이어 위치 나타내기
                map[pl.y][pl.x] = '@';

                // 7, 8. 맵 표시, 스탯 표시
                PrintMap(map, tiles, pl, mapInfo, difficulty, step);

                // 9. 전투 이벤트 진입
                if (isInBattle) {

                    system("CLS");

                    if (FightEnemy(pl, battleMonsterType, difficulty)) {
                        isInBattle = false;
                        isMapInit = true;
                        isInMap = true;
                        mapInfo.enemyCount--;
                    } else {
                        isInBattle = false;
                        isInMap = true;
                        isInGame = false;
                    }
                    isMapInit = true;
                }


                //10. (상점 이벤트가 발생했다면) 상점 루프 진입
                if (isInShop) {
                    VisitShop(pl);

                    isInShop = false;
                    isMapInit = true;
                    isInMap = true;
                    
                }

                //11. 열쇠 획득 이벤트
                if (isPlAtKey) {
                    isPlAtKey = false;
                    if (mapInfo.keyCount == 0) {
                        cout << "열쇠를 전부 획득했다!" << endl
                            << "탈출구로 이동하자!" << endl;
                    }
                    else {
                        cout << "열쇠를 획득했다!" << endl
                            << "탈출까지 앞으로 " << mapInfo.keyCount << "개 남았다." << endl;
                    }
                    cout << "****************[확인]****************";
                    _getch();
                    isMapInit = true;
                }

                //12. (승리 이벤트가 발생했다면) 승리
                if (isPlAtGoal) {
                    isPlAtGoal = false;
                    if (mapInfo.keyCount <= 0) {
                        cout << plName << "는 탈출에 성공했습니다. 승리!" << endl;
                        isWin = true;
                        isInGame = false;

                        for (int y = 0; y < mapInfo.mapH; y++) {
                            delete[] map[y];
                        }
                        delete[] map;
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

Player CreatePlayer(string name) {
    Player pl;
    
    pl.x = pl.y = 0;
    pl.hp = 100;
    pl.hp_max = 100;
    pl.hp_max_inc = 10;
    pl.exp = 0;
    pl.exp_max = 80;
    pl.exp_max_inc = 20;
    pl.money = 20;
    pl.level = 1;
    pl.name = name;

    return pl;
}

Tile** CreateTiles(int height, int width) {
    Tile** result = new Tile * [height];

    for (int y = 0; y < height; y++) {
        Tile* mapRow = new Tile[width];
        for (int x = 0; x < width; x++) {
            mapRow[x].x = x;
            mapRow[x].y = y;
            mapRow[x].type = rand() % NUM_TILE_TYPE;
        }
        result[y] = mapRow;
    }
    return result;
}

char** CreateEmptyMap(int height, int width) {
    char** result = new char* [height];

    for (int y = 0; y < height; y++) {
        char* mapRow = new char[width];
        for (int x = 0; x < width; x++) {
            mapRow[x] = ' ';
        }
        result[y] = mapRow;
    }
    return result;
}

MapInfo MapInitChar(char** map,
                 Enemy* enemyList,
                 Shop* shopList,
                 Key* keyList,
                 Goal& goal,
                 int difficulty) {
    
    MapInfo mapInfo = { 0, 0, 0 };

    int randomPos;

    int mapH = DIFFICULTY_TYPES[difficulty].map_h;
    int mapW = DIFFICULTY_TYPES[difficulty].map_w;

    // 탈출구 배치하기
    while (true) {
        int goalPosY = rand() % (mapH / 2) + mapH - (mapH / 2);
        int goalPosX = rand() % (mapW / 2) + mapW - (mapW / 2);
        if (map[goalPosY][goalPosX] == ' ') {
            map[goalPosY][goalPosX] = 'E'; //Exit
            goal.x = goalPosX;
            goal.y = goalPosY;
            break;
        }
        
    }

    // 열쇠 배치하기
    while (mapInfo.keyCount < DIFFICULTY_TYPES[difficulty].mapKeyCount_init) {
        randomPos = rand() % (mapH * mapW - 1) + 1;
        int randomPosY = randomPos / mapH;
        int randomPosX = randomPos % mapW;
        if (map[randomPosY][randomPosX] == ' ') {
            
            map[randomPosY][randomPosX] = 'K'; //Key
            mapInfo.keyCount++;
        }
    }

    // 적 배치하기
    while (mapInfo.enemyCount < DIFFICULTY_TYPES[difficulty].mapEnemyCount_init) {
        randomPos = rand() % (mapH * mapW - 1) + 1;
        int randomPosY = randomPos / mapH;
        int randomPosX = randomPos % mapW;
        if (map[randomPosY][randomPosX] == ' ') {
            
            int monsterType = rand() % 3;
            if (monsterType == 0) {
                map[randomPosY][randomPosX] = 'W'; //Bat (0)
            }
            else if (monsterType == 1) {
                map[randomPosY][randomPosX] = 'M'; //Goblin (1)
            }
            else {
                map[randomPosY][randomPosX] = 'G'; //Demon (2)
            }
            enemyList[mapInfo.enemyCount].x = randomPosX;
            enemyList[mapInfo.enemyCount].y = randomPosY;
            enemyList[mapInfo.enemyCount].type = monsterType;
            enemyList[mapInfo.enemyCount].isAlive = true;
            mapInfo.enemyCount++;
        }
    }

    // 상점 배치하기
    while (mapInfo.shopCount < DIFFICULTY_TYPES[difficulty].mapShopCount_init) {
        randomPos = rand() % (mapH * mapW - 1) + 1;
        int randomPosY = randomPos / mapH;
        int randomPosX = randomPos % mapW;
        if (map[randomPosY][randomPosX] == ' ') {
            
            map[randomPosY][randomPosX] = 'S'; //Shop
            mapInfo.shopCount++;
        }
    }
    return mapInfo;
}

Collision CheckMapChar(int x, int y, char** map) {

	Collision col = { false, false, false, false, false, -1, -1 };

    char symbol = map[y][x];

    for (int i = 0; i < NUM_ENEMY_TYPE;  i++) {
        if (symbol == ENEMY_TYPES[i].symbol) {
            col.isEnemy = true;
            col.enemyType = i;
       }
    }
	switch (symbol) {
    case '@': {
        col.isPlayer = true;
        break;
    }
	case 'S': {
		col.isShop = true;
		break;
	}
	case 'E': {
		col.isExit = true;
		break;
	}
	case 'K': {
		col.isKey = true;
		break;
	}
	default: {
		break;
	}

	}
	return col;
}

void MoveAllEnemy(char** map, Tile** tiles, Enemy enemyList[], MapInfo mapInfo) {
    //for (int i = 0; i < mapInfo.enemyCount_init; i++) {
    //    cout << enemyList[i].x << " , " << enemyList[i].y << endl;
    //}
    for (int i = 0; i < mapInfo.enemyCount_init; i++) {
        int originX = enemyList[i].x;
        int originY = enemyList[i].y;
        int checkX;
        int checkY;
        if (enemyList[i].isAlive) { // 랜덤 방향 정하기
            do {
                checkX = enemyList[i].x;
                checkY = enemyList[i].y;
                int direction = rand() % 4;
                switch (direction) {
                case 0:
                    checkX += 1;
                    break;

                case 1:
                    checkY += 1;
                    break;

                case 2:
                    checkX -= 1;
                    break;

                case 3:
                    checkY -= 1;
                    break;

                default:
                    checkX = -1;
                    checkY = -1;
                }
            } while (!((checkX >= 0) && (checkY >= 0) && (checkX < mapInfo.mapW) && (checkY < mapInfo.mapH)));
            //정한 XY값 기준으로 비어있는지 체크
            if (map[checkY][checkX] == ' ' 
                && TILE_TYPES[tiles[checkY][checkX].type].symbol == TILE_TYPES[ENEMY_TYPES[enemyList[i].type].tileType].symbol) {
                map[originY][originX] = ' ';
                map[checkY][checkX] = ENEMY_TYPES[enemyList[i].type].symbol;
                enemyList[i].x = checkX;
                enemyList[i].y = checkY;

            }
        }
    }
    /*
    for (int y = 0; y < mapH; y++) {
        for (int x = 0; x < mapW; x++) {


        }
    }*/
}

bool PlayerMove(Player& pl, MapInfo mapInfo, int input) {
    switch (input) {
    case 'W':
        if (pl.y != 0) { pl.y--; return true; }
        break;
    case 'S':
        if (pl.y != mapInfo.mapH - 1) { pl.y++; return true; }
        break;
    case 'A':
        if (pl.x != 0) { pl.x--; return true; }
        break;
    case 'D':
        if (pl.x != mapInfo.mapW - 1) { pl.x++; return true; }
        break;
    default:
        return false;
        break;
    }
    return false;
}

void PrintMap(char** map, Tile** tiles, Player pl, MapInfo mapInfo, int difficulty, int step) {
    cout << pl.name << "는 절차적              난이도 : ";
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
    for (int j = 0; j < mapInfo.mapW; j++) {
        cout << "━";
    }
    cout << "┓";
    cout << endl;

    int printRow = mapInfo.mapH < 11 ? 13 : mapInfo.mapH + 2;
    for (int i = 0; i < printRow; i++) {

        if (i < mapInfo.mapH) {
            cout << "┃";
            for (int j = 0; j < mapInfo.mapW; j++) {
                cout << ((map[i][j] != ' ') ? map[i][j] : TILE_TYPES[tiles[i][j].type].symbol);
            }
            cout << "┃";
        }
        else if (i == mapInfo.mapH) {
            cout << "┗";
            for (int j = 0; j < mapInfo.mapW; j++) {
                cout << "━";
            }
            cout << "┛";
        }
        else {
            for (int j = 0; j < mapInfo.mapW + 2; j++) {
                cout << " ";
            }
        }

        switch (i)
        {
        case 1:
            cout << "   " << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max;
            break;
        case 2:
            cout << "   " << pl.name << "의 돈 : " << pl.money;
            break;
        case 3:
            cout << "   " << pl.name << "의 레벨 : " << pl.level << " ( " << pl.exp << " / " << pl.exp_max << " )";
            break;
        case 5:
            cout << "   남은 적의 수 : " << mapInfo.enemyCount;
            break;
        case 6:
            cout << "   남은 열쇠의 수 : " << mapInfo.keyCount;
            break;
        case 7:
            cout << "   걸음 수 : " << step;
            break;
        case 9:
            cout << "   K - 열쇠 ";
            break;
        case 10:
            cout << "   S - 상점 ";
            break;
        case 11:
            cout << "   E - 탈출구 ";
            break;
        default:
            break;
        }
        cout << endl;

    }
}

void VisitShop(Player& pl) {
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
            << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        for (int i = 0; i < NUM_POTION_TYPE; i++) {
            cout << i + 1 << ". " << POTION_TYPES[i].name << "(HP + " << POTION_TYPES[i].healAmt << ")";
            cout << " 비용: " << POTION_TYPES[i].price << endl;
        }
        cout << "0. 나가기" << endl
            << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        
        cout << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max << endl;
        cout << pl.name << "의 돈 : " << pl.money << endl;
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
            && pl.money >= POTION_TYPES[selectNum - 1].price
            && pl.hp < pl.hp_max) {

            healHP = POTION_TYPES[selectNum - 1].healAmt;
        }
        else if (pl.money < POTION_TYPES[selectNum - 1].price) {
            isShopNoMoney = true;
        }
        else if (pl.hp == pl.hp_max) {
            isShopHPFull = true;
        }

        if (selectNum != -1) {
            if (healHP != 0 && selectNum >= 1) {
                pl.money -= POTION_TYPES[selectNum - 1].price;
                healHP = (pl.hp + healHP < pl.hp_max) ? healHP : pl.hp_max - pl.hp;
                pl.hp += healHP;
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
}

bool FightEnemy(Player& pl, int battleMonsterType, int difficulty) {
    bool result = false;

    //가위바위보 전투
    cout << ENEMY_TYPES[battleMonsterType].name << "이 나타났다." << endl
        << ENEMY_TYPES[battleMonsterType].name << "의 공격력 : "
        << ENEMY_TYPES[battleMonsterType].attackValue << endl
        << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max << endl
        << "전투 시작!" << endl;
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;

    int enemyAttackVal = ENEMY_TYPES[battleMonsterType].attackValue;

    string RSP[3] = { "가위", "바위", "보" };
    string RSP_alt[3] = { "1", "2", "3" };

    bool winFight = false;
    bool loseFight = false;

    int turns = 0;

    while (!winFight && !loseFight)
    {
        int enemyChoice = rand() % 3;
        string plInput;
        int plChoice = -1;
        int rspResult;

        turns++;

        while (plChoice == -1) {
            cout << "가위(1), 바위(2), 보(3) 중 하나를 입력하세요 : ";
            cin >> plInput;

            for (int i = 0; i < 3; i++) {
                if (plInput == RSP[i] || plInput == RSP_alt[i]) {
                    plChoice = i;
                    break;
                }
            }
        }

        cout << "당신의 입력 -> " << RSP[plChoice] << " VS "
            << RSP[enemyChoice] << " <- " << ENEMY_TYPES[battleMonsterType].name << "의 입력 " << endl;

        //0: 비김 1: 이김 2: 짐 
        rspResult = plChoice - enemyChoice;
        rspResult += (plChoice - enemyChoice < 0) ? 3 : 0;

        switch (rspResult) {
        case 0: { // 비김
            cout << "무승부!" << endl;
            cout << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max << endl;

            break;
        } case 1: { // 이김
            cout << pl.name << "의 공격!" << endl
                << ENEMY_TYPES[battleMonsterType].name << "을 쓰러뜨렸다!" << endl;

            winFight = true;

            break;
        } case 2: { // 짐
            cout << ENEMY_TYPES[battleMonsterType].name << "의 공격!" << endl
                << pl.name << "는 " << enemyAttackVal << " 피해를 입었다!" << endl;
            pl.hp = (pl.hp - enemyAttackVal >= 0) ? pl.hp - enemyAttackVal : 0;
            cout << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max << endl;

            if (pl.hp == 0) {
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
        int moneyGain_max = ENEMY_TYPES[battleMonsterType].moneyGain_max;
        int moneyGain_min = ENEMY_TYPES[battleMonsterType].moneyGain_min;
        
        int moneyGain = (rand() % (moneyGain_max + 1 - moneyGain_min) + moneyGain_min)
            * DIFFICULTY_TYPES[difficulty].moneyGainCoeff;
        int expGain = ENEMY_TYPES[battleMonsterType].expGain + turns;

        result = true;
        
        cout << "전투에서 승리했다! 돈을 " << moneyGain << " 획득했다!" << endl;
        cout << pl.name << "의 체력 : " << pl.hp << " / " << pl.hp_max << endl;
        cout << pl.name << "의 돈 : " << pl.money << " + " << moneyGain << endl;
        cout << pl.name << "의 경험치 : " << pl.exp << " + " << expGain
            << " / " << pl.exp_max << endl;

        pl.money += moneyGain;
        pl.exp += expGain;

        if (pl.exp >= pl.exp_max) {
            pl.level++;
            cout << "레벨이 " << pl.level << " 가 되었다!" << endl
                << "최대 체력이 " << pl.hp_max_inc << " 증가했다!" << endl;

            pl.exp -= pl.exp_max;
            pl.exp_max += pl.exp_max_inc;
            pl.hp_max += pl.hp_max_inc;
            pl.hp += pl.hp_max_inc;
        }
    }
    else if (loseFight) {
        result = false;

        cout << pl.name << "가 쓰러졌다..." << endl;
        cout << "패배!" << endl;
    }
    cout << "****************[확인]****************";
    _getch();
    return result;
}