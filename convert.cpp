#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
// A simple structure to hold information about a pending transformation
struct Transform {
  std::string
      marker;  // e.g., "*" for italic, "**" for bold, "__" for underline etc.
  bool isOpen; // true if we have just opened, false if closing
};

int main(int argc, char *argv[]) {
    for (int i=0; i< argc; i++) {
        cout << "param "<<i<<"; "<<argv[i]<<endl;
    
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

  bool nextLineRecent = true;
  int headingLevel = 0;


  // A lambda to process a markerBuffer if nonempty and then clear it.
  auto processMarkerBuffer = [&]() {
    if (markerBuffer.empty())
      return;
    // Decide what transformation to apply depending on markerBuffer content
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
    } else
    if (!codeOpen) {
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


  auto processNewLine = [&]() {
    if (headingLevel != 0) {
        output += " [/h"+std::to_string(headingLevel)+"]";
        headingLevel = 0;
    }
    if (inLinkUrl) {
        output += "["+linkUrl+"]";
        inLinkUrl = false;
        linkUrl.clear();
    }
    if (inLinkText) {
        output += "["+linkText+"]";
        inLinkText = false;
        linkText.clear();

    }
    processMarkerBuffer();
    markerBuffer.clear();
      nextLineRecent = true;
      return;
};


  while (inFile.get(curr)) {
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
      markerBuffer.push_back(curr);
      break;
    case '[':
      processMarkerBuffer();
      // Before switching to link mode flush any pending markers.
      // Start link text accumulation. We'll delay output until we get the full
      if (!codeOpen && !noparseOpen) {
        // link.
        inLinkText = true;
        linkText.clear();
      } else {
        output.push_back(curr);
      }
      break;
    case ']':
      if (!codeOpen && !noparseOpen && inLinkText) {
        // End of link text. Expecting '(' next for URL.
        inLinkText = false;
        // We output nothing yet. The URL part should follow.
      } else {
        // If we're not in a link text, treat as literal.
        processMarkerBuffer();
        output.push_back(curr);
      }
      break;
    case '(':
        if (!codeOpen && !noparseOpen &&!inLinkUrl && !inLinkText && !linkText.empty()) {
          // After link text, this indicates the start of a URL.
          inLinkUrl = true;
          linkUrl.clear();
        } else {
          processMarkerBuffer();
          output.push_back(curr);
        }
        break;
    case ')':
        if (!codeOpen && !noparseOpen &&inLinkUrl) {
          // End of link URL. Output a steam-style link.
          inLinkUrl = false;
          // For demonstration, we output [link=url]text[/link]
          output += "[url=" + linkUrl + "]" + linkText + "[/url]";
          linkText.clear();
          linkUrl.clear();
        } else {
          processMarkerBuffer();
          output.push_back(curr);
        }
        break;
      case '\n':
      case '\r':
        processNewLine();
    case ' ':
      if ((!markerBuffer.empty()) && (markerBuffer.back() == '#' || markerBuffer.back() == '-')) {
        markerBuffer.push_back(curr);
      }
    //   markerBuffer.clear();

    default:
      // Any other character:
      // If we are in link text or link URL, accumulate accordingly.
      if (inLinkText) {
        cout << "adding to text " << curr<<endl;
        linkText.push_back(curr);
      } else if (inLinkUrl) {
        cout << "adding to url " << curr<<endl;
        linkUrl.push_back(curr);
      } else {
        // Not in a link: flush any pending markers, then set this character as
        // literal.
        processMarkerBuffer();
        // SECTION 2: Append the current (literal) character to output.
        output.push_back(curr);
      }
      break;
    }
    // End of while iteration.
  }

  // At end-of-file:
  // If link accumulation was left open, flush it literally.
  if (inLinkText) {
    // We have an unclosed link – output the [ and its text literally.
    output.push_back('[');
    output += linkText;
  }
  if (inLinkUrl) {
    output.push_back('(');
    output += linkUrl;
  }
  // Flush any pending markers.
  processMarkerBuffer();

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

  inFile.close();
  outFile.close();

  std::cout << "Transformation complete. Output written to " << argv[2]
            << std::endl;

  return 0;
}
