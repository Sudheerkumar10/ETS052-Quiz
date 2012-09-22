#include <iostream>
#include <windows.h>
#include "ConsoleManager.h"
#include "QuestionReader.h"
#include "QuizSession.h"
#include "QuestionManager.h"
#include <sstream>
#include <map>
#include <algorithm>
#include <locale>

using namespace std;
using namespace ETS052;

ConsoleManager cm;
QuestionManager qm;

typedef vector<int> ScoreTable;

void Header() 
{
    cm.clearScreen();
    cm.print("--      Created by  -- ", Black , DarkRed);
    cm.print(" ETS052 - Computer Communication ", DarkRed, Red);
    cm.print(" --         Zol       --", Red, DarkBlack);
    cm.print("\n", 0);
}

char DrawMenu() 
{
    Header();

    cm.print("\n", 0);
    cm.print("\n", 0);
    cm.print("\n", 0);

    cm.print("                       ", DarkBlue, DarkBlack);
    cm.print(" MENU ", DarkBlue, Green);
    cm.print("                       ", DarkBlue, DarkBlack); cm.print("\n", DarkBlack);
    cm.print("                       ", Black);
    cm.print("[ 1 ] ", 10); cm.print("New Session", DarkBlack, DarkWhite); cm.print("\n", DarkBlack);
    cm.print("                       ", DarkBlack);
    cm.print("[ 2 ] ", 10); cm.print("Quit", DarkBlack, DarkWhite); cm.print("\n", DarkBlack);

    return cm.read();
}

void PrintResult( ScoreTable *st )
{
    int width = cm.getBufferWidth();
    int count = 0;
    for (auto it = st->begin(); it != st->end(); it++)
    {
        if (*it)
            cm.print(" ", Green, Green);
        else
            cm.print(" ", Red, Red);
        if (count == width)
            cm.print("\n");
        count = (count + 1) % width;
    }
    cm.print("\n", DarkBlack, DarkBlack);
}

void ReadQuestions( QuizSession qs ) 
{
    QuestionReader qr;
    for (int i = 1; i < 33; i++)
    {
        stringstream ss;
        ss << "questions/" << i << ".txt";
        cm.print("Parsing..." + ss.str(), 128);
        qr.parseQuestions(i, &qm, ss.str());
    }
}

void PrintCategories(map<int, string> &chosenCategories, map<int, string> &selectableCategories) 
{
    cm.print("Selected Categories\n", 154);

    for (auto it = chosenCategories.begin(); it != chosenCategories.end(); it++)
    {
        stringstream ss;
        ss << it->first;
        if (it->first < 10)
            cm.print("[ ", DarkWhite, Green);
        else 
            cm.print("[", DarkWhite, Green);
        cm.print(ss.str(), DarkWhite, White);
        cm.print("]", DarkWhite, Green);
        cm.print(" ", DarkBlack, Black);
        cm.print(it->second, Black, DarkWhite);
        cm.print("\n", 0);
    }

    cm.print("Selectable Categories\n", 154);

    for (auto it = selectableCategories.begin(); it != selectableCategories.end(); it++)
    {
        stringstream ss;
        ss << it->first;
        if (it->first < 10)
            cm.print("[ ",  DarkWhite, Green);
        else 
            cm.print("[",  DarkWhite, Green);
        cm.print(ss.str(), DarkWhite, White);
        cm.print("]",  DarkWhite, Green);
        cm.print(" ",  DarkBlack, Black);
        cm.print(it->second, Black, DarkWhite);
        cm.print("\n", DarkBlack);
    }
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int SelectChapters( QuizSession &qs, QuestionManager &qm ) 
{
    Header();

    map<int, string> chosenCategories;
    map<int, string> selectableCategories;

    for (int i = 0; i < 32; i++)
    {
        selectableCategories[i] = qm.getCategory(i);
    }

    string *res;
    while(true)
    {
        Header();
        PrintCategories(chosenCategories, selectableCategories);

        cm.print("Type number followed by enter to select a chapter:", Black, DarkRed);

        res = cm.readLine();

        if (is_number(*res))
        {
            int id = atoi(res->c_str());
            if (id >= 0 && id <= 32)
            {
                auto c = selectableCategories.find(id);
                if (c != selectableCategories.end())
                {
                    chosenCategories[c->first] = c->second;
                    selectableCategories.erase(c);
                }
                else
                {
                    c = chosenCategories.find(id);
                    if (c != chosenCategories.end())
                    {
                        selectableCategories[c->first] = c->second;
                        chosenCategories.erase(c);
                    }
                    else
                    {
                        cout << "COCOCOCOCOCOMBOBREAKER" << endl;
                    }
                }
                
            }
        }
        else
        {
            if (res->compare("c") == 0)
            {
                return -1;
            }
            else if (res->compare("s"))
            {
                for(auto it = chosenCategories.begin(); it != chosenCategories.end(); it++)
                {
                    qs.addQuestions(qm.getQuestionSet(it->first));
                }
                return 1;
            }
        }
        delete res;
    }
}

void PrintScoreHeader(ScoreTable *st) 
{
    Header();
    PrintResult(st);
}

AnswerSheet PrintQuestion( Question * q ) 
{
    cm.print("[", Green , DarkGreen);
    cm.print(q->getCategory(), DarkGreen, Green);
    cm.print("]\n", Green , DarkGreen);
    cm.print(q->getQuestion(), Black, Blue);
    cm.print("\n", Black, Blue);
    cm.print("\n", Black, Blue);

    AnswerSheet answerSheet = q->shuffleOptions();
    auto answer = answerSheet.first;
    auto options = answerSheet.second;
    int i = 1;
    stringstream ss;
    for(auto it = options.begin(); it != options.end(); it++)
    {
        ss << i++;
        cm.print("[", DarkGreen, Green);
        cm.print(ss.str(), DarkCyan, Green);
        cm.print("]", DarkGreen, Green);
        cm.print(" ", DarkBlack);
        cm.print(*it, DarkRed, DarkWhite);
        cm.print(" ", DarkBlack);
        cm.print("\n", DarkBlack);
        ss.str(string());
        ss.clear();
    }

    return answerSheet;
}

int AnswerQuestion( Question * q, AnswerSheet answerSheet ) 
{
    stringstream ss;   

    while(!is_number(ss.str()))
    {
        ss.str(string());
        ss.clear();
        cm.print("Type the option number to answer this question: ", Black, DarkRed);

        char res = cm.read();
        ss << res;

        if (res == 'q')
            exit(0);
    }

        return atoi(ss.str().c_str()) - 1 == answerSheet.first;
}



void DoQuiz( QuizSession &qs ) 
{
    Header();

    //stringstream ss;
    Question *q;
    qs.startQuiz();
    ScoreTable st;
    string previousAnswer;
    while(qs.hasQuestions())
    {
        cm.clearScreen();

        q = qs.nextQuestion();

        PrintScoreHeader(&st);

        cm.print("Previous answer was: ", DarkRed, Yellow);
        cm.print(previousAnswer, Red, DarkRed);
        cm.print("\n", DarkBlack);

        cm.print("Next question: \n", 138);
        cm.print("\n", 136);
        auto answerSheet = PrintQuestion(q);
        previousAnswer = answerSheet.second[answerSheet.first];
        st.push_back( AnswerQuestion(q, answerSheet) );
    }
}



void RepeatQuizOrQuit( QuizSession &qs ) 
{
    throw std::exception("The method or operation is not implemented.");
}

void NewSession() 
{
    Header();
    QuizSession qs;
    ReadQuestions(qs);
    int res = SelectChapters(qs, qm);
    if (res == -1)
    {
        exit(0);
    }
    else 
    {
        DoQuiz(qs);
        RepeatQuizOrQuit(qs);
    }
}

void binary(int number) {
    int remainder;

    if(number <= 1) {
        cout << number;
        return;
    }

    remainder = number%2;
    binary(number >> 1);    
    cout << remainder;
}

int main()
{
//    HANDLE hConsole;
//    int k;

//    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // you can loop k higher to see more color choices
    //for(k = 1; k < 255; k++)
    //{
    //    // pick the colorattribute k you want
    //    SetConsoleTextAttribute(hConsole, k);
    //    cout << k << " I want to be nice today!" << endl;
    //}
    
    //QuestionReader qr;
    //qr.parseQuestions(1, &qm, "questions/1.txt");

    //cm.clearScreen();

    //for (int i = 0; i < 255; i++)
    //{
    //    cout << i << ": ";
    //    binary(i);
    //    cout << "\n";
    //}

    while (true)
    {
        char res = DrawMenu();

        /*QuestionManager qm;
        QuestionReader qr;
        qr.parseQuestions(&qm, "questions/1.txt");*/

        if (res == '1') {
            NewSession();
        }

        if (res == '2')
            exit(0);
    }
    cin.get();
    return 0;
}