#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

void print_help(std::string s) {
  cout << "Usage:\n" << s << " input.md [output]";
}

int main(int argc, char *argv[]) {
  ifstream input;
  if (argc < 2) {
    print_help(argv[0]);
    goto awoo;
    // exit(1);
  }

  if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
    print_help(argv[0]);
    exit(0);
  }

  if (!filesystem::exists(argv[1])) {
    cerr << "File " << argv[1] << " does not exist.";
    exit(1);
  }

  if (false) {
  awoo:
    input = ifstream("test.md");
  } else {
    input = ifstream(argv[1]);
  }
  std::stringstream buffer;

  if (!input) {
    std::cerr << "Error opening file." << std::endl;
    exit(1);
  } else {
    std::string line;
    bool currentlyBold = false;
    bool currentlyUnderlined = false;
    bool currentlyStriked = false;
    bool currentlyNoparse = false;
    bool currentlyItalic = false;
    int noparseLength = 0;
    int offset = 0;
    string urlBase = "";

    while (std::getline(input, line)) { // Read line by line
      std::regex noparsepattern(R"((\`+))");
      // sample : ``text eee `t` should not be noparse`` but the rest should
      /*
      while true
        if currentlynoparse
            look out for noparsepatten $2 AFTER offset chars with EXACTLY
      noparseLength of length if found, replace with $1[/noparse]$2 and move
      offset to the location of the $2 we found and reset currentlyNoparse and
      the noparseLength else look out for any matching noparsepatten $2 AFTER
      offset chars if found, set currentlyNoparse to true and noparseLength to
      the size of our $2 match


      */
      size_t currentPos = 0; // Start searching from the current offset
      while (currentPos < line.length()) {
        // cin.get();
        std::smatch match;
        string ts = line.substr(currentPos);
        if (std::regex_search(ts, match, noparsepattern)) {
          cout << "debug??: found codeblock in " << ts << endl;
          for (int i = 0; i < 2; i++) {
            cout << "el" << i << ": " + match.str(i) << endl;
          }
          if (currentlyNoparse) {
            if (match[1].length() == noparseLength) {
              cout << "valid length" << endl;
              string repl = "[/code]";
              if (noparseLength < 3)
                repl = "[/noparse]";

              string linestart = line.substr(0, currentPos + match.position(1));
              cout << "prestring " + linestart << endl;
              line =
                  linestart + repl +
                  line.substr(currentPos + match.position(1) + match.length(1));
              cout << "poststring " + line << endl;

              currentPos += match.position(1) + repl.size();
              currentlyNoparse = false;
              noparseLength = 0;
            } else {
              cout << "invalid length" << endl;
              currentPos += match[1].length();
              continue;
              ;
            }
          } else {
            currentlyNoparse = true;
            noparseLength = match[1].length();
            string repl = "[code]";
            if (noparseLength < 3)
              repl = "[noparse]";

            string linestart = line.substr(0, currentPos + match.position(1));
            cout << "prestring " + linestart << endl;
            line =
                linestart + repl +
                line.substr(currentPos + match.position(1) + match[1].length());
            cout << "poststring " + line << endl;

            currentPos += match.position(1) + repl.size();
          }
        } else {
          // No more matches found, break the loop
          break;
        }
      }
      cout << "code blocks done";

      while (regex_match(line, noparsepattern)) {
        if (currentlyStriked) {
          line = std::regex_replace(line, noparsepattern, "$1[/strike]$2");
        } else {
          line = std::regex_replace(line, noparsepattern, "$1[strike]$2");
        }
        currentlyStriked = !currentlyStriked;
      }

      std::regex h1pattern(R"(# (.*))");
      std::regex h2pattern(R"(## (.*))");
      std::regex h3pattern(R"(### (.*))");

      line = std::regex_replace(line, h3pattern, "[h3] $1 [/h3]");
      line = std::regex_replace(line, h2pattern, "[h2] $1 [/h2]");
      line = std::regex_replace(line, h1pattern, "[h1] $1 [/h1]");

      std::regex boldpattern(R"((.*)\*\*(.*))");
      while (regex_match(line, boldpattern)) {
        currentlyBold = !currentlyBold;
        if (currentlyBold) {
          line = std::regex_replace(line, boldpattern, "$1[/b]$2");
        } else {
          line = std::regex_replace(line, boldpattern, "$1[b]$2");
        }
      }

      std::regex underlinedpattern(R"((.*)__(.*))");
      while (regex_match(line, underlinedpattern)) {
        currentlyUnderlined = !currentlyUnderlined;
        if (currentlyUnderlined) {
          line = std::regex_replace(line, underlinedpattern, "$1[/u]$2");
        } else {
          line = std::regex_replace(line, underlinedpattern, "$1[u]$2");
        }
      }

      std::regex underlinedhtml(R"((.*)<u>(.*))");
      while (regex_match(line, underlinedhtml)) {
        line = std::regex_replace(line, underlinedhtml, "$1[u]$2");
      }
      std::regex underlinedhtmlclose(R"((.*)</u>(.*))");
      while (regex_match(line, underlinedhtmlclose)) {
        line = std::regex_replace(line, underlinedhtmlclose, "$1[/u]$2");
      }

      std::regex strikedpattern(R"((.*)~~(.*))");
      currentlyStriked = !currentlyStriked;
      while (regex_match(line, strikedpattern)) {
        if (currentlyStriked) {
          line = std::regex_replace(line, strikedpattern, "$1[/strike]$2");
        } else {
          line = std::regex_replace(line, strikedpattern, "$1[strike]$2");
        }
      }


      std::regex italicpattern(R"((.*)\*(.*))");
      currentlyItalic = !currentlyItalic;
      while (regex_match(line, italicpattern)) {
        if (currentlyItalic) {
          line = std::regex_replace(line, italicpattern, "$1[/i]$2");
        } else {
          line = std::regex_replace(line, italicpattern, "$1[i]$2");
        }
      }
      std::regex italicpattern2(R"((.*)_(.*))");
      currentlyItalic = !currentlyItalic;
      while (regex_match(line, italicpattern)) {
        if (currentlyItalic) {
          line = std::regex_replace(line, italicpattern2, "$1[/i]$2");
        } else {
          line = std::regex_replace(line, italicpattern2, "$1[i]$2");
        }
      }

      std::regex linebreakpattern(R"(^(----*$))");
      if (regex_match(line, linebreakpattern)) {
        line = "[hr][/hr]";
      }


      std::regex urlPrefixStorepattern(R"(<!-- (.*) -->)");
      if (regex_match(line, urlPrefixStorepattern)) {
        urlBase = std::regex_replace(line, urlPrefixStorepattern, "$1");
        continue;
      }



      std::regex imagepattern(R"(!\[(.*)\]\((.*)\))");
      std::smatch match;
      while (regex_match(line, match, imagepattern)) {
        string prefixadd = "";
        if (strcmp(match.str(1).substr(0,4).c_str(), "http") != 0 ) prefixadd = urlBase+"/";
        line = std::regex_replace(line, imagepattern, "[img] "+prefixadd+"$2 [/img]");
      }
      // [video] https://codeberg.org/catsoft/RainWorldMods/media/branch/main/BackgroundPreview/images/demo.webm [/video]



      std::regex urlpattern(R"(\[(.*)\]\((.*)\))");
      while (regex_match(line, match, urlpattern)) {
        string prefixadd = "";
        if (strcmp(match.str(1).substr(0,4).c_str(), "http") != 0 ) prefixadd = urlBase+"/";
        line = std::regex_replace(line, urlpattern, "[url=$2] "+prefixadd+"$1 [/url]");
      }

      std::regex linebk(R"((.*)(\\$))");
      if (regex_match(line, linebk)) {
        line = std::regex_replace(line, linebk, "$1");
      }

      cout << "processed " + line << endl;
      // cin.get();

      buffer << line << endl;
    }
    std::cout << buffer.str() << std::endl; // Process the line (e.g., print it)
    string outFileName = "output.steammd";
    if (argc > 2)
      outFileName = argv[2];
    std::ofstream ofs(outFileName);

    if (ofs.is_open()) {
      ofs << buffer.rdbuf();
      ofs.close();
    } else {
      std::cerr << "Unable to write to file" + outFileName + "\n";
    }
  }

  return 0;
}
