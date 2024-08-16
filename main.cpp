#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>
#include <tuple>

void DebugPrint(std::string Content)
{
#ifdef _DEBUG
    std::cout << Content;
#endif
}

bool TREAT_WARNINGS_AS_FATAL = true; // stop the interpretor if a warning is encountered
bool DISABLE_CASE_SENSITIVE_FUNCTIONS = true; // ability to call functions despite capital letters
bool KEYWORD_RESERVE_NUM = false; // ability to make variables with number identifiers (Var 1 = 2)
bool InStatement = false;

std::map<std::string, std::string> Variables = {};

std::string ReadFile(const std::string& FileName) {
    std::ifstream File(FileName);
    std::stringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str();
}

std::vector<std::string> SplitIntoLines(const std::string& Content) 
{
    std::vector<std::string> Lines;
    std::stringstream Stream(Content);
    std::string Line;
    while (std::getline(Stream, Line))
        Lines.push_back(Line);
    return Lines;
}

std::vector<std::string> SplitIntoSpaces(const std::string& Content) 
{
    std::vector<std::string> Result;
    std::istringstream Stream(Content);
    std::string Token;
    while (std::getline(Stream, Token, ' ')) 
        Result.push_back(Token);
    return Result;
}

std::vector<std::string> SplitByQuotes(const std::string& Content) 
{
    std::vector<std::string> Result;
    size_t Start = 0, End;

    while ((Start = Content.find('"', Start)) != std::string::npos) 
    {
        End = Content.find('"', Start + 1);
        if (End == std::string::npos)
            break;
        Result.push_back(Content.substr(Start + 1, End - Start - 1));
        Start = End + 1;
    }

    return Result;
}

std::string ReplaceSpacesBetweenQuotes(std::string Content) 
{
    bool InsideQuotes = false;
    for (char& Character : Content) 
    {
        if (Character == '"') 
            InsideQuotes = !InsideQuotes;
        else if (InsideQuotes && Character == ' ') 
            Character = -1;
    }
    return Content;
}

std::string ReplaceTildesWithSpaces(std::string Content) 
{
    for (char& Character : Content) 
        if (Character == -1)
            Character = ' ';
    return Content;
}

std::string DeleteSurroundingBrackets(std::string Content)
{
    if (Content.empty())
        return Content;
    if (Content[0] == '(')
        Content = Content.substr(1);
    if (Content.back() == ')')
        Content = Content.substr(0, Content.size() - 1);

    return Content;
}

bool TokenIsString(std::string Content)
{
    if (Content.empty())
        return false;
    if (Content[0] == '"' || Content.back() == '"')
        return true;

    return false;
}

std::string DeleteSurroundingQuotes(std::string Content)
{
    if (Content.empty())
        return Content;
    if (Content[0] == '"')
        Content = Content.substr(1);
    if (Content.back() == '"')
        Content = Content.substr(0, Content.size() - 1);

    return Content;
}

int FindNextEndBrace(std::vector<std::string> Lines, int StartLine = 0)
{
    for (int LineNumber = StartLine; LineNumber < Lines.size(); LineNumber++)
    {
        std::vector<std::string> NextLineTokens = SplitIntoSpaces(Lines[LineNumber]);
        if (NextLineTokens.size() > 0 && NextLineTokens[0].find('}') != std::string::npos) // next line has a closing brace
            return LineNumber;
    }

    return -1;
}

std::string RemoveStartingSpaces(std::string Content)
{
    size_t FirstSpacePosition = Content.find_first_not_of(" \t");
    if (FirstSpacePosition != std::string::npos) 
        Content.erase(0, FirstSpacePosition);

    return Content;
}

std::string ToLower(std::string Content)
{
    for (int CharIndex = 0; CharIndex < Content.size() - 1; CharIndex++)
        Content[CharIndex] = tolower(Content[CharIndex]);
    return Content;
}

int FindNextTokenWithEndBracket(std::vector<std::string> Tokens)
{
    for (int TokenIndex = 0; TokenIndex < Tokens.size(); TokenIndex++)
        if (Tokens[TokenIndex].find(')') != std::string::npos)
            return TokenIndex;
    return -1;
}

std::string GetTokensUntilCharSplitBy(std::vector<std::string> Tokens, const char Until, const char Splitter)
{
    for (int TokenIndex = 0; TokenIndex < Tokens.size(); TokenIndex++)
        if (Tokens[TokenIndex].find(Until) != std::string::npos)
        {
            Tokens[TokenIndex].substr(Until);
        }
        else
    return "";
}

int RemoveNextOfChar(std::vector<std::string> Tokens)
{
    for (int TokenIndex = 0; TokenIndex < Tokens.size(); TokenIndex++)
        if (Tokens[TokenIndex].find(')') != std::string::npos)
            return TokenIndex;
    return -1;
}

void Log(std::string Content, std::string Grade = "Info")
{
    std::cout << "[ " << Grade << " ] | " << Content << std::endl;
}

void Log(std::string Content, std::string Grade, int LineNumber, std::string Token)
{
    Content = "Line: " + std::to_string(LineNumber + 1) + ", Token: \"" + Token + "\": " + Content + "";
    Log(Content, Grade);
}

std::tuple<bool, std::string> EvaluateValue(std::vector<std::string> Tokens, int LineNumber)
{
    for (int TokenNumber = 0; TokenNumber < Tokens.size(); TokenNumber++)
        if (Variables.find(Tokens[TokenNumber]) != Variables.end())
            Tokens[TokenNumber] = Variables[Tokens[TokenNumber]];

    std::string FinalValue = Tokens[0];

    if (TokenIsString(FinalValue))
    {
        FinalValue = DeleteSurroundingQuotes(FinalValue);
        FinalValue = ReplaceTildesWithSpaces(FinalValue);
    }

    for (int TokenIndex = 1; TokenIndex < Tokens.size(); TokenIndex++)
    {
        std::string Operator = Tokens[TokenIndex];
        std::string OtherValue = Tokens[TokenIndex + 1];

        if (TokenIsString(OtherValue))
        {
            OtherValue = DeleteSurroundingQuotes(OtherValue);
            OtherValue = ReplaceTildesWithSpaces(OtherValue);
        }

        bool BothAreInt = false;

        if (Variables.find(OtherValue) != Variables.end())
            OtherValue = Variables[OtherValue];
        if (Variables.find(FinalValue) != Variables.end())
            FinalValue = Variables[FinalValue];

        try {
            int ValueA = std::stoi(FinalValue);
            int ValueB = std::stoi(OtherValue);
            BothAreInt = true;
        }
        catch (const std::invalid_argument& Error) {
            BothAreInt = false;
        }
        catch (const std::out_of_range& Error) {
            BothAreInt = false;
        }

        if (Operator == "+")
        {
            if (BothAreInt)
            {
                int IntValue = stoi(FinalValue);
                IntValue += stoi(OtherValue);
                FinalValue = std::to_string(IntValue);
            }
            else
                FinalValue += OtherValue;
        }
        else if (Operator == "*")
        {
            if (BothAreInt)
            {
                int ValueA = stoi(FinalValue);
                int ValueB = stoi(OtherValue);
                FinalValue = std::to_string(ValueA * ValueB);
            }
            else
            {
                Log("Attempt to multiply non-integer.", "Warn", LineNumber, Operator);
                if (TREAT_WARNINGS_AS_FATAL) return std::make_tuple(false, FinalValue);
            }
        }
        else if (Operator == "/")
        {
            if (BothAreInt)
            {
                int ValueA = stoi(FinalValue);
                int ValueB = stoi(OtherValue);
                if (ValueB == 0)
                {
                    Log("Attempt to divide by 0.", "Warn", LineNumber, Operator);
                    if (TREAT_WARNINGS_AS_FATAL) return std::make_tuple(false, FinalValue);
                }
                FinalValue = std::to_string(ValueA / ValueB);
            }
            else
            {
                Log("Attempt to divide non-integer.", "Warn", LineNumber, Operator);
                if (TREAT_WARNINGS_AS_FATAL) return std::make_tuple(false, FinalValue);
            }
        }

        TokenIndex++;
    }

    return std::make_tuple(true, FinalValue);
}

std::string LiterallyRemoveSpaces(std::string Content) // and tabs
{
    for (int CharIndex = 0; CharIndex < Content.size(); CharIndex++)
    {
        if (Content[CharIndex] == ' ' || Content[CharIndex] == '/t')
            Content.erase(Content.begin() + CharIndex);
    }

    return Content;
}

// removes surrounding brackets
std::vector<std::string> SplitParameters(std::string Content) // ex: (VarA, VarB) -> { VarA, VarB }
{
    Content = LiterallyRemoveSpaces(DeleteSurroundingBrackets(Content)); // removes spaces and ( )
    std::vector<std::string> Parameters = std::vector<std::string>();

    size_t Start = 0;
    size_t End = Content.find(',');

    while (End != std::string::npos)
    {
        Parameters.push_back(Content.substr(Start, End - Start));
        Start = End + 1;
        End = Content.find(',', Start);
    }

    Parameters.push_back(Content.substr(Start));

    return Parameters;
}

void ToggleDefine(std::string Input, bool* Define, std::string DebugName = "")
{
    if (Input == "true" || stoi(Input) == 1)
        *Define = true;
    else if (Input == "false" || stoi(Input) == 0)
        *Define = false;

    if (!DebugName.empty()) DebugPrint(std::format("Set {} to {}\n", DebugName, *Define ? "true" : "false"));
}

bool DebugTimerRunning = false;
std::chrono::steady_clock::time_point DebugTimerStart;
void DebugTimer()
{
#ifdef _DEBUG
    if (!DebugTimerRunning)
    {
        DebugTimerStart = std::chrono::high_resolution_clock::now();
        DebugTimerRunning = true;
    }
    else
    {
        auto DebugTimerEnd = std::chrono::high_resolution_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(DebugTimerEnd - DebugTimerStart);
        DebugPrint("Finished in: " + std::to_string(Duration.count()) + "ms");
    }
#endif
}

int main(int ArgumentCount, char* ArgumentValues[]) {
    DebugTimer();
    DebugPrint("Ling: [ Debug ]\n");

    std::string FileName = "";
    std::string FullScript = "";
    std::vector<std::string> Lines = std::vector<std::string>();

    if (ArgumentCount == 1)
    {
        Log("No Input File Provided.", "Error");
        return 1;
    }

    FullScript = ReplaceSpacesBetweenQuotes(ReadFile(ArgumentValues[1]));
    Lines = SplitIntoLines(FullScript);

    for (int LineNumber = 0; LineNumber < Lines.size(); LineNumber++)
    {
        std::string Line = Lines[LineNumber];
        std::vector<std::string> Tokens = SplitIntoSpaces(Lines[LineNumber]);

        for (int TokenNumber = 0; TokenNumber < Tokens.size(); TokenNumber++)
        {
            std::string Token = Tokens[TokenNumber];
            Token = RemoveStartingSpaces(Token); // for indentation

            if (Token.find("(") != std::string::npos && FindNextTokenWithEndBracket(Tokens) != -1) // FUNCTION
            {
                size_t StartParenthesis = Token.find('(');
                size_t EndParenthesis = Token.find(')');

                std::string FunctionName = Token.substr(0, StartParenthesis);
                if (DISABLE_CASE_SENSITIVE_FUNCTIONS) FunctionName = ToLower(FunctionName);
                std::string FuncInput = DeleteSurroundingBrackets(Token.substr(StartParenthesis));
                std::vector<std::string> Arguments = SplitParameters(FuncInput);

                if (FunctionName == "PrintLine" || DISABLE_CASE_SENSITIVE_FUNCTIONS && FunctionName == "printline")
                {
                    std::vector<std::string> Parameter = SplitByQuotes(Arguments[0]);
                    if (Variables.find(Arguments[0]) != Variables.end()) // did find a variable with name
                        std::cout << Variables[Arguments[0]] << std::endl;
                    else if (Parameter.empty())
                    {
                        Log("Missing content to print or missing quotes.", "Error", LineNumber, Token);
                        return 1;
                    }
                    else
                        std::cout << ReplaceTildesWithSpaces(Parameter[0]) << std::endl;
                }
                else if (FunctionName == "Print" || DISABLE_CASE_SENSITIVE_FUNCTIONS && FunctionName == "print")
                {
                    std::vector<std::string> Parameter = SplitByQuotes(Arguments[0]);
                    if (Variables.find(Arguments[0]) != Variables.end()) // did find a variable with name
                        std::cout << Variables[Arguments[0]];
                    else if (Parameter.empty())
                    {
                        Log("Missing content to print or missing quotes.", "Error", LineNumber, Token);
                        return 1;
                    }
                    else
                        std::cout << ReplaceTildesWithSpaces(Parameter[0]);
                }
                else
                {
                    Log("Unknown function.", "Error", LineNumber, Token);
                    continue;
                }
            }
            else if ( (Token.find("(") != std::string::npos && Token.find(")") == std::string::npos) || (Token.find("(") == std::string::npos && Token.find(")") != std::string::npos) ) // BROKEN FUNCTION
            {
                Log("Missing a starting or ending function bracket.", "Error", LineNumber, Token);
                return 1;
            }
            else if ( (Token == "Variable" || Token == "Var") && Tokens.size() > (TokenNumber + 3))
            {
                if (KEYWORD_RESERVE_NUM)
                {
                    try {
                        stoi(Tokens[TokenNumber + 1]);
                        Log("Attempt to redefine reserved keyword (Number).", "Error", LineNumber, Token); // if we reached here without catching an error thats bad
                        return 1;
                    }
                    catch (const std::invalid_argument& Error) {
                        // nothing because this is expected
                    }
                }

                std::string FinalValue = Tokens[TokenNumber + 3];
                if (TokenIsString(FinalValue))
                {
                    FinalValue = DeleteSurroundingQuotes(FinalValue);
                    FinalValue = ReplaceTildesWithSpaces(FinalValue);
                }

                for (int TokenIndex = 4; TokenIndex < Tokens.size() - 1; TokenIndex++)
                {
                    std::vector<std::string> TokenListForEvaluation;
                                                                                                   //  0   1   2    3
                    for (int EvalTokenIndex = 3; EvalTokenIndex < Tokens.size(); EvalTokenIndex++) // var iden = firstval <- start here
                        TokenListForEvaluation.push_back(Tokens[EvalTokenIndex]);

                    bool Success;
                    std::tie(Success, FinalValue) = EvaluateValue(TokenListForEvaluation, LineNumber);
                    if (!Success) return 1;
                }

                DebugPrint("Created variable \"" + Tokens[TokenNumber + 1]  + "\" with value: " + FinalValue + "\n");
                Variables[Tokens[TokenNumber + 1]] = FinalValue;
                break;
            }
            else if (Token == "If")
            {
                int LinesToSkip = 0; // so it doesnt interpret the actual if statement logic code
                if (Tokens.size() >= 5) // If ValueA == ValueB Run
                {
                    
                    std::string ValueA = Tokens[1]; // ValueA
                    std::string ComparisonSymbol = Tokens[2];
                    std::string ValueB = Tokens[3]; // ValueB, which is after the comparison symbol
                    
                    if (Variables.find(ValueA) != Variables.end())
                        ValueA = Variables[ValueA];
                    if (Variables.find(ValueB) != Variables.end())
                        ValueB = Variables[ValueB];
                    
                    InStatement = true;
                    std::vector<std::string> NextLineTokens = SplitIntoSpaces(Lines[LineNumber + 1]);
                    int StartLogicCode = 0;
                    if (NextLineTokens.size() > 0 && NextLineTokens[0].find('{') != std::string::npos) // next line has an opening brace
                    {
                        LinesToSkip++;
                        StartLogicCode = LineNumber + 1; // line after the opening brace
                        int ClosingBraceLine = FindNextEndBrace(Lines, StartLogicCode);
                        if (ClosingBraceLine == -1) // not found
                        {
                            Log("No closing brace for logic code.", "Warn", LineNumber, Token);
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else LineNumber += LinesToSkip; break; // skip to end of if statement logic code
                        }

                        bool ConditionMet = (ComparisonSymbol == "==" && ValueA == ValueB) || (ComparisonSymbol == "!=" && ValueA != ValueB);

                        if (ConditionMet)
                            LineNumber = StartLogicCode; // start interpreting the logic code like normal
                        else
                            LineNumber = ClosingBraceLine + 1; // line after the closing brace and skip the logic code
                    }
                    else
                    {
                        Log("No opening brace for logic code.", "Warn", LineNumber, Token);
                        if (TREAT_WARNINGS_AS_FATAL) return 1; else LineNumber += LinesToSkip; break; // skip to end of if statement logic code
                    }
                }
                else
                {
                    Log("Incorrect If statement arguments.", "Warn", LineNumber, Token);
                    if (TREAT_WARNINGS_AS_FATAL) return 1; else LineNumber += LinesToSkip; break; // skip to end of if statement logic code
                }
            }
            else if (Token == "Quit" && Tokens.size() > 1) // Quit [CODE]
            {
                std::cout << Line << std::endl;
                try {
                    int Code = stoi(Tokens[1]); // try parse code as int
                    return Code; // exit with code
                }
                catch (const std::invalid_argument& Error) {
                    Log("Invalid exit code.", "Warn", LineNumber, Token);
                    if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                }
            }
            else if (Token == "DEFINE" && Tokens.size() > (TokenNumber + 2))
            {
                try {
                    if (Tokens[TokenNumber + 1] == "TREAT_WARNINGS_AS_FATAL")
                        ToggleDefine(Tokens[TokenNumber + 2], &TREAT_WARNINGS_AS_FATAL, "TREAT_WARNINGS_AS_FATAL");
                    else if (Tokens[TokenNumber + 1] == "DISABLE_CASE_SENSITIVE_FUNCTIONS")
                        ToggleDefine(Tokens[TokenNumber + 2], &DISABLE_CASE_SENSITIVE_FUNCTIONS, "DISABLE_CASE_SENSITIVE_FUNCTIONS");
                    else if (Tokens[TokenNumber + 1] == "KEYWORD_RESERVE_NUM")
                        ToggleDefine(Tokens[TokenNumber + 2], &KEYWORD_RESERVE_NUM, "KEYWORD_RESERVE_NUM");
                }
                catch (const std::invalid_argument& Error) {
                    Log("Attempt to define bool as otherwise.", "Warn", LineNumber, Token);
                    if (TREAT_WARNINGS_AS_FATAL) return 1;
                }
                break;
            }
            else if (Variables.find(Token) != Variables.end()) // this is an existing variable, asking for reassignment?
            {
                if (Tokens[TokenNumber + 1] == "=")
                {
                    if (Tokens.size() == 3) // identifier, equals, value
                    {
                        if (Variables.find(Tokens[TokenNumber + 2]) != Variables.end())
                            Variables[Token] = Variables[Tokens[TokenNumber + 2]]; // this var = other var
                        else
                            Variables[Token] = Tokens[TokenNumber + 2]; // value
                    }
                }
            }
            else if (Token == "//")
                break;
        }
    }

    DebugTimer();
    return 0;
}