#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <regex>
#include <string>
#include <vector>

using namespace std;
// A simple structure to hold information about a pending transformation
struct Transform {
  std::string
      marker;  // e.g., "*" for italic, "**" for bold, "__" for underline etc.
  bool isOpen; // true if we have just opened, false if closing
};

std::string output;
char curr;
// Buffer for accumulating marker characters (if they come in series)
std::string markerBuffer;

// State for link parsing
bool inLinkText = false;
std::string linkText;
bool inLinkUrl = false;
std::string linkUrl;

// Transformation open flags for each type – we use these to toggle output
// tags.
bool italicOpen = false;
bool boldOpen = false;
bool underlineOpen = false;
bool strikethroughOpen = false;
bool codeOpen = false;
bool spoilerOpen = false;
bool noparseOpen = false;
bool nextLinkIsMedia = false;

bool nextLineRecent = true;
int headingLevel = 0;

// A lambda to process a markerBuffer if nonempty and then clear it.
void processMarkerBuffer() {
  if (markerBuffer.empty())
    return;
  // Decide what transformation to apply depending on markerBuffer content

  std::string withoutSpaces;
  vector<int> diffIndexes;

  if (markerBuffer.length() != 1) {
    // Copy non-space characters from original to withoutSpaces
    std::copy_if(markerBuffer.begin(), markerBuffer.end(),
                 std::back_inserter(withoutSpaces),
                 [](char c) { return c != ' '; });

    for (int i = 1; i < withoutSpaces.length(); i++) {
      if (withoutSpaces[i - 1] != withoutSpaces[i]) {
        diffIndexes.push_back(i);
        cout << "marker discrepancy : " << withoutSpaces[i - 1] << "vs"
             << withoutSpaces[i] << " at " << i << endl;
      }
    }
  }
  if (diffIndexes.size() > 0) {
    cout << "more than one symbol in symbols ! " << withoutSpaces << endl;
    int lastpos = 0;
    for (auto index : diffIndexes) {
      markerBuffer = withoutSpaces.substr(lastpos, index - lastpos);
      cout << "forcing processing " + markerBuffer << "." << endl;
      processMarkerBuffer();
      lastpos += index - lastpos;
    }
    markerBuffer = withoutSpaces.substr(lastpos);
    cout << "finishing processing inconsistency w " << markerBuffer << endl;

    //   return;
  }

  

  if (markerBuffer == "```") {
    if (!codeOpen) {
      output += "[code]";
      codeOpen = true;
    } else {
      output += "[/code]";
      codeOpen = false;
    }
  } else if (markerBuffer == "``") {
    if (!noparseOpen) {
      output += "[noparse]";
      noparseOpen = true;
    } else {
      output += "[/noparse]";
      noparseOpen = false;
    }
  } else if (markerBuffer == "`") {
    if (!noparseOpen) {
      output += "[noparse]";
      noparseOpen = true;
    } else {
      output += "[/noparse]";
      noparseOpen = false;
    }
  } else if (!codeOpen && !noparseOpen) {
    if (markerBuffer == "*") {
      if (!italicOpen) {
        output += "[i]";
        italicOpen = true;
      } else {
        output += "[/i]";
        italicOpen = false;
      }
    } else if (markerBuffer == "**") {
      if (!boldOpen) {
        output += "[b]";
        boldOpen = true;
      } else {
        output += "[/b]";
        boldOpen = false;
      }
    } else if (markerBuffer == "_") {
      // Single underscore: treat as literal
      output += "_";
    } else if (markerBuffer == "__") {
      if (!underlineOpen) {
        output += "[u]";
        underlineOpen = true;
      } else {
        output += "[/u]";
        underlineOpen = false;
      }
    } else if (markerBuffer == "~") {
      // Single tilde: literal
      output += "~";
    } else if (markerBuffer == "~~") {
      if (!strikethroughOpen) {
        output += "[strike]";
        strikethroughOpen = true;
      } else {
        output += "[/strike]";
        strikethroughOpen = false;
      }
    } else if (markerBuffer == "|") {
      // Single pipe: literal
      output += "|";
    } else if (markerBuffer == "||") {
      if (!spoilerOpen) {
        output += "[spoiler]";
        spoilerOpen = true;
      } else {
        output += "[/spoiler]";
        spoilerOpen = false;
      }
    } else if (nextLineRecent && markerBuffer == "- ") {
      output += "[*]";
    } else if (nextLineRecent && markerBuffer == "# ") {
      headingLevel = 1;
      output += "[h1]";
    } else if (nextLineRecent && markerBuffer == "## ") {
      headingLevel = 2;
      output += "[h2]";
    } else if (nextLineRecent && markerBuffer == "### ") {
      headingLevel = 3;
      output += "[h3]";
    } else if (strcmp(markerBuffer.substr(0, 3).c_str(), "---") == 0) {
      output += "[hr][/hr]";
    } else {
      // Unhandled marker sequence: output as-is
      output += markerBuffer;
    }
  } else {
    // in code block
    output += markerBuffer;
  }

  markerBuffer.clear();
  nextLineRecent = false;
};

static inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

bool startsWith(const std::basic_string<char> &str,
                const std::basic_string<char> &prefix) {
  if (prefix.size() > str.size()) {
    return false;
  }
  return str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string &str, const std::string &suffix) {
  if (suffix.size() > str.size()) {
    return false;

    // cout<<"comparing "<<str.substr(str.size() - suffix.size(),
    // suffix.size()+1)<<" to "<<suffix<<endl;
  }
  return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void resetLinksStuff(string reason="") {
  cout << "flush links "+reason << endl;
  if (!linkText.empty() || !linkUrl.empty()) {
    cout << "flushing nonemptylinks with status " << inLinkText << " "
         << inLinkUrl << " " << linkText << "|||" << linkUrl << endl;
  }
  if (!linkText.empty())
    output += "[" + linkText + "]";
  if (!linkUrl.empty())
    output += "(" + linkUrl + ")";
  inLinkText = false;
  inLinkUrl = false;
  linkUrl.clear();
  linkText.clear();
  nextLinkIsMedia = false;
}

void processNewLine() {
  if (headingLevel != 0) {
    output += " [/h" + std::to_string(headingLevel) + "]";
    headingLevel = 0;
  }
  resetLinksStuff("because new line");
  processMarkerBuffer();
  markerBuffer.clear();
  nextLineRecent = true;
  return;
};

int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    cout << "param " << i << "; " << argv[i] << endl;
  }
  std::ifstream inFile("README.md");
  std::ofstream outFile("out.md");

  //   if (argc < 3) {
  //     std::cerr << "Usage: " << argv[0]
  //               << " <input_markdown_file> <output_steam_file>" << std::endl;
  //     return 1;
  //   }

  //   std::ifstream inFile(argv[1]);
  //   if (!inFile) {
  //     std::cerr << "Error: Unable to open input file." << std::endl;
  //     return 1;
  //   }

  //   std::ofstream outFile(argv[2]);
  //   if (!outFile) {
  //     std::cerr << "Error: Unable to open output file." << std::endl;
  //     return 1;
  //   }
  string mediaUrlPrefix;
  std::stringstream matchingBuffer;
  matchingBuffer << inFile.rdbuf();
  std::string content = matchingBuffer.str();
  inFile.close();

  // Example: regex match for words "example"
  std::regex commentPropertyPattern(R"(<!-- mediaUrlPrefix:(.*) -->)");
  std::smatch match;

  if (std::regex_search(content, match, commentPropertyPattern)) {
    mediaUrlPrefix = match.str(1);
    cout << "Set url prefix to" + mediaUrlPrefix << endl;
  }

  std::regex commentPattern(R"(<!-- (.*) -->)");
  if (regex_search(content, match, commentPattern)) {
    cout << "matched for comments" << endl;

    content = std::regex_replace(content, commentPattern,
                                 "[table comment:$1][/table]");
  }

  std::regex endLineJumpIgnorePattern(R"((\\\n))");
  content = std::regex_replace(content, endLineJumpIgnorePattern, "\n");

  //   cout << content<<endl<<endl;

  std::stringstream preProcessedBuffer(content.c_str());

  //   inFile.close();

  //   inFile = ifstream("README.md");

  while (preProcessedBuffer.get(curr)) {
    // SECTION 1: Process the current character (using a switch for flexibility)

    switch (curr) {
    case '*':
    case '_':
    case '~':
    case '`':
    case '|':
    case '#':
    case '-':
      // These are all possible marker characters. Accumulate them.
      
    if (!inLinkText && !inLinkUrl) {
        markerBuffer.push_back(curr);
        break;
      } else {
        if (inLinkText) {
            // cout << "adding to text " << curr<<endl;
            linkText.push_back(curr);
          } else if (inLinkUrl) {
            // cout << "adding to url " << curr<<endl;
            linkUrl.push_back(curr);
          } 
          break;
      }
      
    case '[':
      processMarkerBuffer();
      // link.
      // Before switching to link mode flush any pending markers.
      // Start link text accumulation. We'll delay output until we get the full
      if (!codeOpen && !noparseOpen) {
        cout << "STRAT fricking link comment" << endl;
        if (!output.empty() && output.back() == '!') {
          nextLinkIsMedia = true;
          cout << "found a start of linked media" << endl;
        }
        if (!linkText.empty()) {
          cout << "whoops. multiple link-like followed each other.\n";
          resetLinksStuff("because open new bracket");
        }
        inLinkText = true;
      } else {
        output.push_back(curr);
      }
      break;
    case ']':
      if (!codeOpen && !noparseOpen && inLinkText) {
        // End of link text. Expecting '(' next for URL.
        inLinkText = false;
        cout << "end fricking link comment" << endl;
        // We output nothing yet. The URL part should follow.
        if (preProcessedBuffer.peek() != '(') {
          cout << "post-end was not parenthesis, it was"
               << preProcessedBuffer.peek() << " discarding link " + linkText
               << endl;
          resetLinksStuff(" because close bracket and no parent");
          //   output += "[" + linkText + "]";
        }
      } else {
        // If we're not in a link text, treat as literal.
        processMarkerBuffer();
        output.push_back(curr);
      }
      break;
    case '(':
      if (!codeOpen && !noparseOpen && !inLinkUrl && !inLinkText &&
          (!linkText.empty())) {
        // After link text, this indicates the start of a URL.
        inLinkUrl = true;
        linkUrl.clear();
      } else {
        resetLinksStuff("because new paren but also no link text");
        processMarkerBuffer();
        output.push_back(curr);
      }
      break;
    case ')':
      if (!codeOpen && !noparseOpen && inLinkUrl) {
        // End of link URL. Output a steam-style link.
        inLinkUrl = false;
        trim(linkUrl);
        trim(linkText);
        cout << "end processing link :" + linkUrl
             << " linked media: " << nextLinkIsMedia << endl;
        if (!mediaUrlPrefix.empty() && !startsWith(linkUrl, "http")) {
          linkUrl = mediaUrlPrefix + linkUrl;
        }
        if (nextLinkIsMedia) output.pop_back(); // remove the !
        if (linkUrl.size() < 4 ||
            (!endsWith(linkUrl, "webm"))) { // !endsWith(linkUrl, "avif") &&  broken i think
          output += "[url=" + linkUrl + "]" + linkText + "[/url]";
        } else {
          output += "[video]" + linkUrl + "[/video]";
        }
        linkText.clear();
        linkUrl.clear();
      } else {
        processMarkerBuffer();
        output.push_back(curr);
      }
      break;
    case '\\':
      output += preProcessedBuffer.get();
      break;
    case '\n':
    case '\r':
      processNewLine();
    case ' ':
      if ((!markerBuffer.empty()) &&
          (markerBuffer.back() == '#' || markerBuffer.back() == '-')) {
        markerBuffer.push_back(curr);
      }
      //   markerBuffer.clear();

    default:
      // Any other character:
      // If we are in link text or link URL, accumulate accordingly.
      if (inLinkText) {
        // cout << "adding to text " << curr<<endl;
        linkText.push_back(curr);
      } else if (inLinkUrl) {
        // cout << "adding to url " << curr<<endl;
        linkUrl.push_back(curr);
      } else {
        // Not in a link: flush any pending markers, then set this character as
        // literal.
        nextLinkIsMedia = false;
        processMarkerBuffer();
        // SECTION 2: Append the current (literal) character to output.
        output.push_back(curr);
      }
      break;
    }

    // End of while iteration.
  }
  processNewLine();

  // It might be wise to also close any open transformation – in a real parser
  // you may want to handle this differently.
  if (italicOpen) {
    output += "[/i]";
  }
  if (boldOpen) {
    output += "[/b]";
  }
  if (underlineOpen) {
    output += "[/u]";
  }
  if (strikethroughOpen) {
    output += "[/s]";
  }
  if (codeOpen) {
    output += "[/c]";
  }
  if (noparseOpen) {
    output += "[/np]";
  }

  // Write the processed output to the steam-flavored file.
  outFile << output;

  outFile.close();

  std::cout << "Transformation complete. Output written to " << argv[2]
            << std::endl;

  return 0;
}
