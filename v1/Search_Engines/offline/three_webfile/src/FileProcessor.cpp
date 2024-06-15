#include "../include/FileProcessor.h"
#include "../include/tinyxml2.h"
#include "../include/PageLib.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace tinyxml2;

int FileProcessor::process(const string &fileName) {
    XMLDocument doc;
    doc.LoadFile(fileName.c_str());

    int index_now = _pageLib.getWebPage().size();
    if (doc.LoadFile(fileName.c_str()) == XML_SUCCESS) {
        XMLElement* channelElement = doc.FirstChildElement("rss")->FirstChildElement("channel");
        XMLElement* itemElement = channelElement->FirstChildElement("item");

        while (itemElement != nullptr) {
            WebPage tmpWP;
            XMLElement* titleElement = itemElement->FirstChildElement("title");
            XMLElement* linkElement = itemElement->FirstChildElement("link");
            XMLElement* descriptionElement = itemElement->FirstChildElement("description");

            string linkTxt, titleTxt, descriptionTxt;
            if (linkElement) {
                linkTxt = linkElement->GetText();
            }
            linkTxt = "<![CDATA[" + linkTxt + "]]>";

            if (titleElement) {
                titleTxt = titleElement->GetText();
                std::regex pattern2("&\\$<br>&\\$");                   // Match the special string to be removed
                titleTxt = std::regex_replace(titleTxt, pattern2, ""); // Replace the matched part with an empty string
            }
            titleTxt = "<![CDATA[" + titleTxt + "]]>";

            if (descriptionElement) {
                descriptionTxt = descriptionElement->GetText();
                std::regex tagRegex("<[^>]*>");
                descriptionTxt = std::regex_replace(descriptionTxt, tagRegex, ""); // Remove HTML tags
                std::regex pattern("&nbsp;");
                descriptionTxt = std::regex_replace(descriptionTxt, pattern, ""); // Remove &nbsp;
                descriptionTxt.erase(std::remove(descriptionTxt.begin(), descriptionTxt.end(), ' '), descriptionTxt.end()); // Remove spaces
            }
            descriptionTxt = "<![CDATA[" + descriptionTxt + "]]>";

            tmpWP._docId = index_now++;
            tmpWP._docTitle = titleTxt;
            tmpWP._docUrl = linkTxt;
            tmpWP._docContent = descriptionTxt;

            _pageLib.getWebPage().push_back(tmpWP);

            itemElement = itemElement->NextSiblingElement("item");
        }
    } else {
        cout << "Failed to load xml file." << endl;
        return -1;
    }

    return 0;
}

