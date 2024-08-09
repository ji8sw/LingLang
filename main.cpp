#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <chrono>

void DebugPrint(std::string Content)
{
#ifdef _DEBUG
    std::cout << Content;
#endif
}

bool TREAT_WARNINGS_AS_FATAL = true;
bool DISABLE_CASE_SENSITIVE_FUNCTIONS = true;
bool InStatement = false;

std::map<std::string, std::string> Variables =
{

};


std::string ReadFile(const std::string& FileName) {
    std::ifstream File(FileName);
    std::stringstream Buffer;
    Buffer << File.rdbuf();
    return Buffer.str();
}

std::vector<std::string> SplitIntoLines(const std::string& Content) {
    std::vector<std::string> Lines;
    std::stringstream Stream(Content);
    std::string Line;
    while (std::getline(Stream, Line))
        Lines.push_back(Line);
    return Lines;
}

std::vector<std::string> SplitIntoSpaces(const std::string& Content) {
    std::vector<std::string> Result;
    std::istringstream Stream(Content);
    std::string Token;
    while (std::getline(Stream, Token, ' ')) {
        Result.push_back(Token);
    }
    return Result;
}

std::vector<std::string> SplitByQuotes(const std::string& Content) {
    std::vector<std::string> Result;
    size_t Start = 0, End;

    while ((Start = Content.find('"', Start)) != std::string::npos) {
        End = Content.find('"', Start + 1);
        if (End == std::string::npos)
            break;
        Result.push_back(Content.substr(Start + 1, End - Start - 1));
        Start = End + 1;
    }

    return Result;
}

std::string ReplaceSpacesBetweenQuotes(std::string Content) {
    bool InsideQuotes = false;
    for (char& Character : Content) {
        if (Character == '"') {
            InsideQuotes = !InsideQuotes;
        }
        else if (InsideQuotes && Character == ' ') {
            Character = '~';
        }
    }
    return Content;
}

std::string ReplaceTildesWithSpaces(std::string Content) {
    for (char& Character : Content) {
        if (Character == '~') {
            Character = ' ';
        }
    }
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
        std::cout << "No File Provided.";
        return 1;
    }

    FullScript = ReplaceSpacesBetweenQuotes(ReadFile(ArgumentValues[1]));
    Lines = SplitIntoLines(FullScript);

    if (FullScript == "" || Lines.empty())
    {
        std::cout << "No Data Recieved.";
        return 1;
    }
    for (int LineNumber = 0; LineNumber < Lines.size(); LineNumber++)
    {
        std::string Line = Lines[LineNumber];
        std::vector<std::string> Tokens = SplitIntoSpaces(Lines[LineNumber]);

        for (int TokenNumber = 0; TokenNumber < Tokens.size(); TokenNumber++)
        {
            std::string Token = Tokens[TokenNumber];
            Token = RemoveStartingSpaces(Token); // for indentation

            if (Token.find("(") != std::string::npos && Token.find(")") != std::string::npos) // FUNCTION
            {
                size_t StartParenthesis = Token.find('(');
                size_t EndParenthesis = Token.find(')');

                std::string FunctionName = Token.substr(0, StartParenthesis);
                if (DISABLE_CASE_SENSITIVE_FUNCTIONS) FunctionName = ToLower(FunctionName);
                std::string Argument = Token.substr(StartParenthesis);

                if (FunctionName == "PrintLine" || DISABLE_CASE_SENSITIVE_FUNCTIONS && FunctionName == "printline")
                {
                    std::vector<std::string> Parameter = SplitByQuotes(Argument);
                    if (Variables.find(DeleteSurroundingBrackets(Argument)) != Variables.end()) // did find a variable with name
                        std::cout << Variables[DeleteSurroundingBrackets(Argument)] << std::endl;
                    else if (Parameter.empty())
                    {
                        std::cout << "Line " << (LineNumber + 1) << " Token " << TokenNumber << ": " << Token << " is incorrect, missing content to print or missing quotes.";
                        return 1;
                    }
                    else
                        std::cout << ReplaceTildesWithSpaces(Parameter[0]) << std::endl;
                }
                else if (FunctionName == "Print" || DISABLE_CASE_SENSITIVE_FUNCTIONS && FunctionName == "print")
                {
                    std::vector<std::string> Parameter = SplitByQuotes(Argument);
                    if (Variables.find(DeleteSurroundingBrackets(Argument)) != Variables.end()) // did find a variable with name
                        std::cout << Variables[DeleteSurroundingBrackets(Argument)];
                    else if (Parameter.empty())
                    {
                        std::cout << "Line " << (LineNumber + 1) << " Token " << TokenNumber << ": " << Token << " is incorrect, missing content to print or missing quotes.";
                        return 1;
                    }
                    else
                        std::cout << ReplaceTildesWithSpaces(Parameter[0]);
                }
                else
                {
                    std::cout << "Line " << (LineNumber + 1) << ", Token " << TokenNumber << ": \"" << FunctionName << "\" is incorrect: unkown function.";
                    continue;
                }
            }
            else if ( (Token.find("(") != std::string::npos && Token.find(")") == std::string::npos) || (Token.find("(") == std::string::npos && Token.find(")") != std::string::npos) ) // BROKEN FUNCTION
            {
                std::cout << "Line " << (LineNumber + 1) << " Token " << TokenNumber << ": " << Token << " is incorrect, missing a starting or ending function bracket.";
                return 1;
            }
            else if ( (Token == "Variable" || Token == "Var") && Tokens.size() > (TokenNumber + 3))
            {
                std::string FinalValue = Tokens[TokenNumber + 3];
                if (TokenIsString(FinalValue))
                {
                    FinalValue = DeleteSurroundingQuotes(FinalValue);
                    FinalValue = ReplaceTildesWithSpaces(FinalValue);
                }

                for (int TokenIndex = 4; TokenIndex < Tokens.size() - 1; TokenIndex++)
                {
                    std::string OtherValue = Tokens[TokenIndex + 1];

                    if (TokenIsString(OtherValue))
                    {
                        OtherValue = DeleteSurroundingQuotes(OtherValue);
                        OtherValue = ReplaceTildesWithSpaces(OtherValue);
                    }

                    if (Tokens[TokenIndex] == "+")
                    {
                        bool BothAreInt = false;
                        
                        if (Variables.find(OtherValue) != Variables.end())
                            OtherValue = Variables[OtherValue];
                        if (Variables.find(FinalValue) != Variables.end())
                            FinalValue = Variables[FinalValue];

                        try {
                            int ValueA = std::stoi(FinalValue);
                            int ValueB = std::stoi(OtherValue);
                            BothAreInt = true; // we reached here so we're ok
                        }
                        catch (const std::invalid_argument& Error) {
                            BothAreInt = false;
                        }
                        catch (const std::out_of_range& Error) {
                            BothAreInt = false;
                        }

                        if (BothAreInt)
                        {
                            int IntValue = stoi(FinalValue);
                            IntValue += stoi(OtherValue);
                            FinalValue = std::to_string(IntValue);
                        }
                        else
                        {
                            FinalValue += OtherValue;
                            TokenIndex += 1;
                        }
                    }
                    else if (Tokens[TokenIndex] == "*")
                    {
                        bool BothAreInt = false;

                        if (Variables.find(OtherValue) != Variables.end())
                            OtherValue = Variables[OtherValue];
                        if (Variables.find(FinalValue) != Variables.end())
                            FinalValue = Variables[FinalValue];

                        try {
                            int ValueA = std::stoi(FinalValue);
                            int ValueB = std::stoi(OtherValue);
                            BothAreInt = true; // we reached here so we're ok
                        }
                        catch (const std::invalid_argument& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                            BothAreInt = false;
                        }
                        catch (const std::out_of_range& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                            BothAreInt = false;
                        }

                        if (BothAreInt)
                        {
                            int ValueA = stoi(FinalValue);
                            int ValueB = stoi(OtherValue);
                            FinalValue = std::to_string(ValueA * ValueB);
                        }
                    }
                    else if (Tokens[TokenIndex] == "/")
                    {
                        bool BothAreInt = false;

                        if (Variables.find(OtherValue) != Variables.end())
                            OtherValue = Variables[OtherValue];
                        if (Variables.find(FinalValue) != Variables.end())
                            FinalValue = Variables[FinalValue];

                        try {
                            int ValueA = std::stoi(FinalValue);
                            int ValueB = std::stoi(OtherValue);
                            BothAreInt = true; // we reached here so we're ok
                        }
                        catch (const std::invalid_argument& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                            BothAreInt = false;
                        }
                        catch (const std::out_of_range& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                            BothAreInt = false;
                        }

                        if (BothAreInt)
                        {
                            int ValueA = stoi(FinalValue);
                            int ValueB = stoi(OtherValue);
                            if (ValueA == 0 || ValueB == 0)
                            {
                                std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to divide by 0.\n";
                                if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                            }
                            FinalValue = std::to_string(ValueA / ValueB);
                        }
                    }
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
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, no closing brace for logic code.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1; else LineNumber += LinesToSkip; break; // skip to end of if statement logic code
                        }

                        if (ComparisonSymbol == "==")
                        {
                            if (ValueA == ValueB)
                            {
                                LineNumber = StartLogicCode; // start interpreting the logic code like normal
                                break;
                            }
                            else
                            {
                                LineNumber = ClosingBraceLine + 1; // line after the closing brace and skip the logic code
                                break;
                            }
                        }
                        else if (ComparisonSymbol == "!=")
                        {
                            if (ValueA != ValueB)
                            {
                                LineNumber = StartLogicCode; // start interpreting the logic code like normal
                                break;
                            }
                            else
                            {
                                LineNumber = ClosingBraceLine + 1; // line after the closing brace and skip the logic code
                                break;
                            }
                        }
                    }
                    else
                    {
                        std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, no opening brace for logic code.\n";
                        if (TREAT_WARNINGS_AS_FATAL) return 1; else LineNumber += LinesToSkip; break; // skip to end of if statement logic code
                    }
                }
                else
                {
                    std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, incorrect If statement arguments.\n";
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
                    std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, invalid exit code.\n";
                    if (TREAT_WARNINGS_AS_FATAL) return 1; else break;
                }
            }
            else if (Token == "DEFINE" && Tokens.size() > (TokenNumber + 2))
            {
                if (Tokens[TokenNumber + 1] == "TREAT_WARNINGS_AS_FATAL")
                {
                    try {
                        if (Tokens[TokenNumber + 2] == "true")
                            TREAT_WARNINGS_AS_FATAL = true;
                        else if (Tokens[TokenNumber + 2] == "false")
                            TREAT_WARNINGS_AS_FATAL = false;
                        else if (stoi(Tokens[TokenNumber + 2]) == 1)
                            TREAT_WARNINGS_AS_FATAL = true;
                        else if (stoi(Tokens[TokenNumber + 2]) == 0)
                            TREAT_WARNINGS_AS_FATAL = false;

                        DebugPrint("Set TREAT_WARNINGS_AS_FATAL to " + std::to_string(TREAT_WARNINGS_AS_FATAL) + "\n");
                    }
                    catch (const std::invalid_argument& Error) {
                        std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to define bool as otherwise.\n";
                        if (TREAT_WARNINGS_AS_FATAL) return 1;
                    }
                }
                else if (Tokens[TokenNumber + 1] == "DISABLE_CASE_SENSITIVE_FUNCTIONS")
                {
                    try {
                        if (Tokens[TokenNumber + 2] == "true")
                            DISABLE_CASE_SENSITIVE_FUNCTIONS = true;
                        else if (Tokens[TokenNumber + 2] == "false")
                            DISABLE_CASE_SENSITIVE_FUNCTIONS = false;
                        else if (stoi(Tokens[TokenNumber + 2]) == 1)
                            DISABLE_CASE_SENSITIVE_FUNCTIONS = true;
                        else if (stoi(Tokens[TokenNumber + 2]) == 0)
                            DISABLE_CASE_SENSITIVE_FUNCTIONS = false;

                        DebugPrint("Set DISABLE_CASE_SENSITIVE_FUNCTIONS to " + std::to_string(DISABLE_CASE_SENSITIVE_FUNCTIONS) + "\n");
                    }
                    catch (const std::invalid_argument& Error) {
                        std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to define bool as otherwise.\n";
                        if (TREAT_WARNINGS_AS_FATAL) return 1;
                    }
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
        }
    }

    DebugTimer();
    return 0;
}   
