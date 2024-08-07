#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <any>

bool TREAT_WARNINGS_AS_FATAL = true;

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
        if (End == std::string::npos) break;
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

std::any GetVariableByName(std::string Variable)
{
    auto Finder = Variables.find(Variable);
    if (Finder != Variables.end())
        return Finder->second;

    return NULL;
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

void DebugPrint(std::string Content)
{
#ifdef _DEBUG
    std::cout << Content;
#endif
}

int main(int ArgumentCount, char* ArgumentValues[]) {
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

            if (Token.find("(") != std::string::npos && Token.find(")") != std::string::npos) // FUNCTION
            {
                size_t StartParenthesis = Token.find('(');
                size_t EndParenthesis = Token.find(')');

                std::string FunctionName = Token.substr(0, StartParenthesis);
                std::string Argument = Token.substr(StartParenthesis);

                if (FunctionName == "PrintLine")
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
            else if (Token == "Variable" && Tokens.size() > (TokenNumber + 3))
            {
                std::string FinalValue = Tokens[TokenNumber + 3];
                for (int TokenIndex = 4; TokenIndex < Tokens.size() - 1; TokenIndex++)
                {
                    std::string OtherValue = Tokens[TokenIndex + 1];

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
                            BothAreInt = true;
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
                            BothAreInt = true;
                        }
                        catch (const std::invalid_argument& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1;
                            BothAreInt = false;
                        }
                        catch (const std::out_of_range& Error) {
                            std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to multiply non-integer.\n";
                            if (TREAT_WARNINGS_AS_FATAL) return 1;
                            BothAreInt = false;
                        }

                        if (BothAreInt)
                        {
                            int ValueA = stoi(FinalValue);
                            int ValueB = stoi(OtherValue);
                            FinalValue = std::to_string(ValueA * ValueB);
                        }
                    }
                }

                DebugPrint("Created variable \"" + Tokens[TokenNumber + 1]  + "\" with value: " + FinalValue + "\n");
                Variables[Tokens[TokenNumber + 1]] = FinalValue;
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

                        DebugPrint("set TREAT_WARNINGS_AS_FATAL to " + std::to_string(TREAT_WARNINGS_AS_FATAL) + "\n");
                    }
                    catch (const std::invalid_argument& Error) {
                        std::cout << "[WARN] Line " << (LineNumber + 1) << " Token " << TokenNumber << " is incorrect, attempt to define bool as otherwise.\n";
                        if (TREAT_WARNINGS_AS_FATAL) return 1;
                    }
                }
            }
        }
    }

    return 0;
}