#include "HelperClassesAndFunctions.h"


osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode)
{
    osg::Group* currGroup;
    osg::Node* foundNode;
    
    if ( !currNode)
    {
        return NULL;
    }
    
    if (currNode->getName() == searchName)
    {
        return currNode;
    }
    
    currGroup = currNode->asGroup();
    if ( currGroup )
    {
        for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
        {
            foundNode = findNamedNode(searchName, currGroup->getChild(i));
            if (foundNode)
                return foundNode;
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

std::vector<std::string> explode(const std::string& str, const char& ch) 
{
    std::string next;
    std::vector<std::string> result;
    
    // For each character in the string
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
        result.push_back(next);
    return result;
}