#include<bits/stdc++.h>
using namespace std;
const static int POSSIBLE_SOLUTIONS_LIMIT = 1;
const static int MAX_WORD_LENGTH = 5;
const static int MAX_GRID_SIDE = 5;
const static int MAX_GRID_CELLS_NO = 25;
const static string ACCROSS_POSITION_ID = "\"accrossNum\":";
const static string DOWN_POSITION_ID = "\"downNum\":";
const static string ACCROSS_CLUES_ID = "\"accross\":";
const static string DOWN_CLUES_ID = "\"down\":";
const static string ANSWER_CLUES_ID = "\"answer\":";
const static string POSITION_CLUES_ID = "\"position\":";
const static string USELESS_TAGS[] = {"with", "like", "at", "the", "of", "in", "on", "a", "that", "to", "an", "it", "is", "this", "for", "from", "and", "or", "as"};
vector<string>words; // container for all the available answers/words
vector<int>across_id; // container for across cell position id
vector<int>down_id; // container for down cell position id
vector<string>across_clues;// container for clues across
vector<string>down_clues;// container for clues down
vector<char>answers; // answer characters
unordered_set<string>used; // lookup for not repeating the same tags for the words
int id_to_cell[MAX_GRID_CELLS_NO + 1] = {0}; // lookup for a cell no given a position id
unordered_map<string, int>tag_match; // tag match count of words/possible solutions
vector<string>tags; // all the available tags
int length_across[MAX_GRID_CELLS_NO + 1] = {0};
int length_down[MAX_GRID_CELLS_NO + 1] = {0};
vector< pair<int, string> >temp_sols;
vector<string>across_solutions[MAX_GRID_CELLS_NO];
vector<string>down_solutions[MAX_GRID_CELLS_NO];
int indexes_for_rec[MAX_GRID_CELLS_NO];
pair<int, int> getCords(int cellNo){
    pair<int, int>cord;
    cord.first = (cellNo / 5) + 1 - (cellNo % 5 == 0 ? 1 : 0);
    cord.second = (cellNo % 5 == 0 ? 5 : cellNo % 5);
    return cord;
}
bool isValid(vector<string>&across_sols, vector<string>&down_sols){
    char puzzle[MAX_GRID_SIDE+1][MAX_GRID_SIDE+1] = {'#', '#', '#', '#', '#', '#',
                                                    '#', '#', '#', '#', '#', '#',
                                                    '#', '#', '#', '#', '#', '#',
                                                    '#', '#', '#', '#', '#', '#',
                                                    '#', '#', '#', '#', '#', '#',
                                                    '#', '#', '#', '#', '#', '#'};
    printf ("PLACING ALL THE ACROSS SOLUTIONS\n");
    for (int i = 0; i < across_sols.size(); i++){
        int cellNo = id_to_cell[across_id[i]];
        int ind = 0;
        while (answers[cellNo] != '#'){
            pair<int, int>cord = getCords(cellNo);
            puzzle[cord.first][cord.second] = across_sols[i][ind];
            ind++;
            cellNo++;
            if ((cellNo - 1) % MAX_GRID_SIDE == 0)
                break;
        }
    }
    printf ("PLACING COMPLETE\n");
    printf ("PLACING ALL THE DOWN SOLUTIONS\n");
    for (int i = 0; i < down_sols.size(); i++){
        int cellNo = id_to_cell[down_id[i]];
        int ind = 0;
        while (answers[cellNo] != '#'){
            pair<int, int>cord = getCords(cellNo);
            if (puzzle[cord.first][cord.second] != down_sols[i][ind]){
                if (puzzle[cord.first][cord.second] == '#')
                    puzzle[cord.first][cord.second] = down_sols[i][ind];
                else{
                    printf ("PLACING ERROR in clue %d", i);
                    cout << " with word " << down_sols[i] << endl;
                    break;
                }
            }
            ind++;
            cellNo += MAX_GRID_SIDE;
            if (cellNo > MAX_GRID_CELLS_NO)
                break;
        }
    }
    printf ("PLACING COMPLETE\n");

    /*printf("\nThe Resulting Grid:\n");
    for (int i = 1; i <= 5; i++){
        for (int j = 1; j <= 5; j++){
            printf ("%c", puzzle[i][j]);
        }
        printf ("\n");
    }*/
    freopen("toSend.txt", "w", stdout);
    int cnt = 0;
    printf ("{\"answer\": [");
    for (int i = 1; i <= 5; i++)
        for (int j = 1; j <= 5; j++){
            cnt++;
            if (i*j == 25){
                if (puzzle[i][j] != '#')
                    printf ("\"%d %c\\n\"", cnt, puzzle[i][j] - 32);
                else
                    printf ("\"%c\"", puzzle[i][j]);
            }
            else{
                if (puzzle[i][j] != '#')
                    printf ("\"%d %c\\n\", ", cnt, puzzle[i][j] - 32);
                else
                    printf ("\"%c\", ", puzzle[i][j]);
            }
        }
    printf ("]}");
    return true;
}
bool rec(int x, int* ind){
    if (x == across_clues.size() + down_clues.size()){
        vector<string>across_sols;
        vector<string>down_sols;
        for (int i = 0; i < across_clues.size(); i++){
            across_sols.push_back(across_solutions[i][ind[i]]);
        }
        for (int i = 0; i < down_clues.size(); i++){
            down_sols.push_back(down_solutions[i][ind[i + across_clues.size()]]);
        }
        if (isValid(across_sols, down_sols))
            return true;
    }
    else{
        if (x < across_clues.size()){
            for (int i = 0; i < across_solutions[x].size(); i++){
                ind[x] = i;
                rec(x+1, ind);
            }
        }
        else{
            for (int i = 0; i < down_solutions[x - across_clues.size()].size(); i++){
                ind[x] = i;
                rec(x+1, ind);
            }
        }
    }
    return false;
}
string toLowerCase(string &s){
    for (int i = 0; i < s.size(); i++){
        if ('A' <= s[i] && s[i] <= 'Z')
            s[i] += 32;
    }
    return s;
}
vector<string> separate(string&s){
    vector<string>tags;
    string curWord = "";
    bool flag = false;
    for (int i = 0; i < s.size(); i++){
        if (s[i] == ' ' || s[i] == ',' || s[i] == '?' || s[i] == '!' || s[i] == '_' || s[i] == '-')
            flag = true;
        else
            flag = false;
        if (flag){
            if (curWord.size() > 0)
                tags.push_back(curWord);
            curWord = "";
            continue;
        }
        curWord.append(1u, s[i]);
    }
    if (curWord.size() > 0)
        tags.push_back(curWord);
    return tags;
}
bool isUselessTag(string &s){
    for (int i = 0; i < 19; i++)
        if (USELESS_TAGS[i].compare(s) == 0)
            return true;
    return false;
}
void filterTags(vector<string>&tags){
    for (int i = 0; i < tags.size(); i++){
        if (isUselessTag(tags[i])){
            tags.erase(tags.begin() + i);
            i--;
        }
    }
}
vector<string> getTags(string &s){
    s = toLowerCase(s);
    vector<string>tags = separate(s);
    filterTags(tags);
    return tags;
}
int main(){
    // filtering the right words
    freopen ("unfilteredWords.txt", "r", stdin);
    freopen ("filteredWords.txt", "w", stdout);

    char curChar;
    string word = "";
    while (scanf("%c", &curChar) != EOF){
        if (curChar != '\n')
            word.append(1u, curChar);
        else{
            if (word.size() <= MAX_WORD_LENGTH){
                words.push_back(word);
            }
            word = "";
        }
    }
    if (word.size() <= MAX_WORD_LENGTH)
        words.push_back(word);

    // test for answers/words
    for (int i = 0; i < words.size(); i++)
        cout << words[i] << endl;

    // getting all data from json file
    // freopen ("store1.json", "r", stdin);
    //freopen ("june12016.json", "r", stdin);
    //freopen ("march6.json", "r", stdin);
    //freopen ("march7.json", "r", stdin);
    //freopen ("october23.json", "r", stdin);
    //freopen ("themedmini.json", "r", stdin);
    //freopen ("Friday  February 2, 2018.json", "r", stdin);
    //freopen ("Friday  February 9, 2018.json", "r", stdin);
    //freopen ("Friday  January 5, 2018.json", "r", stdin);
    //freopen ("Monday  February 5, 2018.json", "r", stdin);
    //freopen ("Monday  January 1, 2018.json", "r", stdin);
    //freopen ("Sunday  February 4, 2018.json", "r", stdin);
    //freopen ("Sunday  January 7, 2018.json", "r", stdin);
    //freopen ("Thursday  February 1, 2018.json", "r", stdin);
    //freopen ("Thursday  February 8, 2018.json", "r", stdin);
    //freopen ("Thursday  January 4, 2018.json", "r", stdin);
    //freopen ("Tuesday  February 6, 2018.json", "r", stdin);
    //freopen ("Tuesday  January 2, 2018.json", "r", stdin);
    //freopen ("Wednesday  February 7, 2018.json", "r", stdin);
    //freopen ("Wednesday  January 3, 2018.json", "r", stdin);
    //freopen ("Wednesday  January 31, 2018.json", "r", stdin);
    //freopen ("Wednesday  January 24, 2018.json", "r", stdin);
    //freopen ("Wednesday  January 17, 2018.json", "r", stdin);
    //freopen ("Wednesday  January 10, 2018.json", "r", stdin);
    //freopen ("Tuesday  January 30, 2018.json", "r", stdin);
    //freopen ("Tuesday  January 23, 2018.json", "r", stdin);
    //freopen ("Tuesday  January 16, 2018.json", "r", stdin);
    //freopen ("Tuesday  January 9, 2018.json", "r", stdin);
    //freopen ("Thursday  January 25, 2018.json", "r", stdin);
    //freopen ("Thursday  January 18, 2018.json", "r", stdin);
    //freopen ("Thursday  January 11, 2018.json", "r", stdin);
    //freopen ("Sunday  January 28, 2018.json", "r", stdin);
    //freopen ("Sunday  January 21, 2018.json", "r", stdin);
    //freopen ("Sunday  January 14, 2018.json", "r", stdin);
    //freopen ("Monday  January 22, 2018.json", "r", stdin);
    //freopen ("Monday  January 15, 2018.json", "r", stdin);
    //freopen ("Monday  January 8, 2018.json", "r", stdin);
    //freopen ("Friday  January 26, 2018.json", "r", stdin);
    //freopen ("Friday  January 19, 2018.json", "r", stdin);
    //freopen ("Friday  January 12, 2018.json", "r", stdin);
    //freopen ("Wednesday  February 28, 2018.json", "r", stdin);
    //freopen ("Wednesday  February 21, 2018.json", "r", stdin);
    //freopen ("Tuesday  February 27, 2018.json", "r", stdin);
    //freopen ("Tuesday  February 20, 2018.json", "r", stdin);
    //freopen ("Tuesday  February 13, 2018.json", "r", stdin);
    //freopen ("Thursday  February 22, 2018.json", "r", stdin);
    //freopen ("Thursday  February 15, 2018.json", "r", stdin);
    //freopen ("Sunday  February 25, 2018.json", "r", stdin);
    freopen ("Sunday  February 18, 2018.json", "r", stdin);
    //freopen ("Sunday  February 11, 2018.json", "r", stdin);
    //freopen ("Monday  February 19, 2018.json", "r", stdin);
    //freopen ("Monday  February 12, 2018.json", "r", stdin);
    //freopen ("Friday  February 16, 2018.json", "r", stdin);
    //freopen ("Monday  February 26, 2018.json", "r", stdin);
    //freopen ("Friday  February 23, 2018.json", "r", stdin);
    //freopen ("Thursday  April 26, 2018.json", "r", stdin);

    freopen ("testJson.txt", "w", stdout);

    string jsonFileRaw = "";
    while (scanf("%c", &curChar) != EOF){
        jsonFileRaw.append(1u, curChar);
    }


    // test for json file retrieval
     cout << "JSON RAW FILE:\n";
     cout << jsonFileRaw << endl;
     cout << endl;

    // getting position values, id_to_cell
    bool found = false;
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, POSITION_CLUES_ID.size());
        if (curSubstr.compare(POSITION_CLUES_ID) == 0){
            found = true;
            int cellNo = 0;
            bool opened = false;
            string curCell = "";
            for (int j = i + POSITION_CLUES_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    cellNo++;
                    int cellId = 0;
                    if (curCell.size() > 2){
                        cellId = curCell[3] - '0';
                        if ('0' <= curCell[4] && curCell[4] <= '9')
                            cellId = (cellId * 10) + curCell[4] - '0';
                    }
                    id_to_cell[cellId] = cellNo;
                    curCell = "";
                }
                if (cellNo >= MAX_GRID_CELLS_NO)
                    break;
            }
        }
        if (found)
            break;
    }

    // test for position values, id_to_cell
     printf("POSITION VALUES:\n");
     for (int i = 1; i <= MAX_GRID_CELLS_NO; i++)
        printf ("%d\n", id_to_cell[i]);
    printf ("\n");

    // getting across position id, across position ids
    found = false;
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, ACCROSS_POSITION_ID.size());
        if (curSubstr.compare(ACCROSS_POSITION_ID) == 0){
            found = true;
            string curCell = "";
            bool opened = false;
            for (int j = i + ACCROSS_POSITION_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    int cellId = 0;
                    cellId = curCell[0] - '0';
                    if (curCell.size() > 1 && '0' <= curCell[1] && curCell[1] <= '9')
                        cellId = (cellId * 10) + curCell[1] - '0';
                    across_id.push_back(cellId);
                    curCell = "";
                }
                if (jsonFileRaw[j] == ']' && jsonFileRaw[j+1] == ']')
                    break;
            }
        }
        if (found)
            break;
    }

    // test
     printf ("ACCROS POSITION IDENTIFIERS:\n");
     for (int i = 0; i < across_id.size(); i++)
        printf ("%d\n", across_id[i]);
    printf ("\n");

    // getting down position id, down position ids
    found = false;
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, DOWN_POSITION_ID.size());
        if (curSubstr.compare(DOWN_POSITION_ID) == 0){
            found = true;
            string curCell = "";
            bool opened = false;
            for (int j = i + DOWN_POSITION_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    int cellId = 0;
                    cellId = curCell[0] - '0';
                    if (curCell.size() > 1 && '0' <= curCell[1] && curCell[1] <= '9')
                        cellId = (cellId * 10) + curCell[1] - '0';
                    down_id.push_back(cellId);
                    curCell = "";
                }
                if (jsonFileRaw[j] == ']' && jsonFileRaw[j+1] == ']')
                    break;
            }
        }
        if (found)
            break;
    }

    // test
    printf ("DOWN POSITION IDENTIFIERS:\n");
    for (int i = 0; i < down_id.size(); i++)
       printf ("%d\n", down_id[i]);
    printf ("\n");

    // getting across clues
    found = false;
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, ACCROSS_CLUES_ID.size());
        if (curSubstr.compare(ACCROSS_CLUES_ID) == 0){
            found = true;
            string curCell = "";
            bool opened = false;
            for (int j = i + ACCROSS_CLUES_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '\\' && jsonFileRaw[j+1] == '"')
                    j += 2;
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    across_clues.push_back(curCell);
                    curCell = "";
                }
                if (jsonFileRaw[j] == ']' && jsonFileRaw[j+1] == ']')
                    break;
            }
        }
        if (found)
            break;
    }

    // test
     printf ("ACROSS CLUES\n");
     for (int i = 0; i < across_clues.size(); i++)
        cout << across_clues[i] << endl;
    printf ("\n");

    // getting down clues
    found = false;
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, DOWN_CLUES_ID.size());
        if (curSubstr.compare(DOWN_CLUES_ID) == 0){
            found = true;
            string curCell = "";
            bool opened = false;
            for (int j = i + DOWN_CLUES_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '\\' && jsonFileRaw[j+1] == '"')
                    j += 2;
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    down_clues.push_back(curCell);
                    curCell = "";
                }
                if (jsonFileRaw[j] == ']' && jsonFileRaw[j+1] == ']')
                    break;
            }
        }
        if (found)
            break;
    }

    // test
    printf ("DOWN CLUES:\n");
    for (int i = 0; i < down_clues.size(); i++)
        cout << down_clues[i] << endl;
    printf ("\n");

    // getting answers
    found = false;
    answers.push_back('#');
    for (int i = 0; i < jsonFileRaw.size(); i++){
        string curSubstr = jsonFileRaw.substr(i, ANSWER_CLUES_ID.size());
        if (curSubstr.compare(ANSWER_CLUES_ID) == 0){
            found = true;
            string curCell = "";
            bool opened = false;
            for (int j = i + ANSWER_CLUES_ID.size(); j < jsonFileRaw.size(); j++){
                if (jsonFileRaw[j] == '"')
                    opened = !opened;
                if (opened && jsonFileRaw[j] != '"'){
                    curCell.append(1u, jsonFileRaw[j]);
                }
                else if (curCell.size() > 0){
                    for (int k = 0; k < curCell.size(); k++)
                        if ('A' <= curCell[k] && curCell[k] <= 'Z' || curCell[k] == '#'){
                            answers.push_back(curCell[k]);
                            break;
                        }
                    curCell = "";
                }
                if (jsonFileRaw[j] == ']')
                    break;
            }
        }
        if (found)
            break;
    }

    // test
    printf("ANSWER CHARACTERS:\n");
    for (int i = 0; i < answers.size(); i++)
        cout << answers[i] << endl;
    printf ("\n");

    // reading previous data
    freopen("tags.txt", "r", stdin);

    string tag = "";
    while (scanf("%c", &curChar) != EOF){
        if (curChar != '\n')
            tag.append(1u, curChar);
        else{
            used.insert(tag + '\n');
            tags.push_back(tag);
            tag = "";
        }
    }
    used.insert(tag + '\n');
    tags.push_back(tag);

    // solving the puzzle

    freopen ("generalTest.txt", "w", stdout);

    // across
    for (int i = 0; i < across_clues.size(); i++){
        int cellNo = id_to_cell[across_id[i]];
        int cnt = 0;
        while (answers[cellNo] != '#'){
            cellNo++;
            cnt++;
            if ((cellNo - 1) % MAX_GRID_SIDE == 0)
                break;
        }
        length_across[i] = cnt;
    }

    // down
    for (int i = 0; i < down_clues.size(); i++){
        int cellNo = id_to_cell[down_id[i]];
        int cnt = 0;
        while (answers[cellNo] != '#'){
            cellNo += MAX_GRID_SIDE;
            cnt++;
            if (cellNo > MAX_GRID_CELLS_NO)
                break;
        }
        length_down[i] = cnt;
    }

    for (int i = 0; i < across_clues.size(); i++){
        vector<string>clueTags = getTags(across_clues[i]);
        tag_match.clear();
        for (int j = 0; j < words.size(); j++){
            tag_match[words[j]] = 0;
        }
        for (int j = 0; j < tags.size(); j++){
            string word = "";
            string tag = "";
            bool flag = false;
            for (int k = 0; k < tags[j].size(); k++){
                if (tags[j][k] == '#'){
                    flag = true;
                    continue;
                }
                if (flag)
                    tag.append(1u, tags[j][k]);
                else
                    word.append(1u, tags[j][k]);
            }
            bool found = false;
            for (int k = 0; k < clueTags.size(); k++){
                if (clueTags[k].compare(tag) == 0){
                    found = true;
                    break;
                }
            }
            if (found){
                tag_match[word]++;
                cout << "MATCH --> ( " << word << ", " << tag << " ) for ACROSS clue " << i + 1 << endl;
            }
        }
        temp_sols.clear();
        for (unordered_map<string,int>::iterator it = tag_match.begin(); it != tag_match.end(); ++it)
            if (it->first.size() == length_across[i])
                temp_sols.push_back({it->second, it->first});
        sort(temp_sols.begin(), temp_sols.end());
        for (int j = temp_sols.size() - 1; j >= max(0, (int)temp_sols.size() - POSSIBLE_SOLUTIONS_LIMIT); j--)
            across_solutions[i].push_back(temp_sols[j].second);
    }

    // test
    printf ("\nACROSS POSSIBLE SOLUTIONS:\n");
    for (int i = 0; i < across_clues.size(); i++){
        cout << "Clue " << i+1 << " ";
        for (int j = 0; j < across_solutions[i].size(); j++)
            cout << across_solutions[i][j] << ", ";
        cout << endl;
    }
    printf ("\n");

    for (int i = 0; i < down_clues.size(); i++){
        vector<string>clueTags = getTags(down_clues[i]);
        tag_match.clear();
        for (int j = 0; j < words.size(); j++){
            tag_match[words[j]] = 0;
        }
        for (int j = 0; j < tags.size(); j++){
            string word = "";
            string tag = "";
            bool flag = false;
            for (int k = 0; k < tags[j].size(); k++){
                if (tags[j][k] == '#'){
                    flag = true;
                    continue;
                }
                if (flag)
                    tag.append(1u, tags[j][k]);
                else
                    word.append(1u, tags[j][k]);
            }
            bool found = false;
            for (int k = 0; k < clueTags.size(); k++){
                if (clueTags[k].compare(tag) == 0){
                    found = true;
                    break;
                }
            }
            if (found){
                tag_match[word]++;
                cout << "MATCH --> ( " << word << ", " << tag << " ) for DOWN clue " << i + 1 << endl;
            }
        }
        temp_sols.clear();
        for (unordered_map<string,int>::iterator it = tag_match.begin(); it != tag_match.end(); ++it)
            if (it->first.size() == length_down[i])
                temp_sols.push_back({it->second, it->first});
        sort(temp_sols.begin(), temp_sols.end());
        for (int j = temp_sols.size() - 1; j >= max(0, (int)temp_sols.size() - POSSIBLE_SOLUTIONS_LIMIT); j--)
            down_solutions[i].push_back(temp_sols[j].second);
    }

    // test
    printf ("\nDOWN POSSIBLE SOLUTIONS:\n");
    for (int i = 0; i < down_clues.size(); i++){
        cout << "Clue " << i+1 << " ";
        for (int j = 0; j < down_solutions[i].size(); j++)
            cout << down_solutions[i][j] << ", ";
        cout << endl;
    }
    printf ("\n");

    rec(0, indexes_for_rec);

    // test + debug
    /*
    vector<string>a;
    vector<string>b;
    a.push_back("ado");
    a.push_back("spec");
    a.push_back("march");
    a.push_back("alias");
    a.push_back("yell");
    b.push_back("april");
    b.push_back("decal");
    b.push_back("ochs");
    b.push_back("sale");
    b.push_back("may");
    isValid(a, b);

    down_solutions[0].push_back("123");
    down_solutions[0].push_back("123");
    int ind[100];
    rec(0, ind);
    */

    // learning emm shallow learning, emm maybe i guess
    // across
    freopen("tags.txt", "a", stdout);
    for (int i = 0; i < across_clues.size(); i++){
        int cellNo = id_to_cell[across_id[i]];
        string curAnswer = "";
        int cnt = 0;
        while (answers[cellNo] != '#'){
            curAnswer.append(1u, answers[cellNo]);
            cellNo++;
            cnt++;
            if ((cellNo - 1) % MAX_GRID_SIDE == 0)
                break;
        }
        length_across[i] = cnt;
        vector<string>tags = getTags(across_clues[i]);
        for (int j = 0; j < tags.size(); j++){
            string newTag = toLowerCase(curAnswer) + "#" + tags[j] + '\n';
            if (used.find(newTag) == used.end()){
                cout << newTag;
            }
        }
    }

    // down
    for (int i = 0; i < down_clues.size(); i++){
        int cellNo = id_to_cell[down_id[i]];
        string curAnswer = "";
        int cnt = 0;
        while (answers[cellNo] != '#'){
            curAnswer.append(1u, answers[cellNo]);
            cellNo += MAX_GRID_SIDE;
            cnt++;
            if (cellNo > MAX_GRID_CELLS_NO)
                break;
        }
        length_down[i] = cnt;
        vector<string>tags = getTags(down_clues[i]);
        for (int j = 0; j < tags.size(); j++){
            string newTag = toLowerCase(curAnswer) + "#" + tags[j] + '\n';
            if (used.find(newTag) == used.end()){
                cout << newTag;
            }
        }
    }

    return 0;
}
