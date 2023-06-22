#include "SVGHelper.h"

void createAttribute(Attribute* newAt, char* attrName, char* attrValue) //used to populate an already initialized attribute
{
    newAt->name = malloc(sizeof(char) * strlen(attrName) + 1); // mallocs space for the attributes name

    strcpy(newAt->name, attrName);
    strcpy(newAt->value, attrValue);
    /*
    strcat(newAt->name, "\0");
    strcat(newAt->value, "\0");
    */
}

SVG * initializeSVG(SVG* newSvg) // used to initialize all the lists in the svg
{
    strcpy(newSvg->namespace, ""); 
    strcpy(newSvg->title, "");
    strcpy(newSvg->description, "");

    newSvg->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    newSvg->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
    newSvg->circles = initializeList(circleToString, deleteCircle, compareCircles);
    newSvg->paths = initializeList(pathToString, deletePath, comparePaths);
    newSvg->groups = initializeList(groupToString, deleteGroup, compareGroups);

    return newSvg;
}

SVG * createSVGTree(xmlDoc * doc)
{
    xmlNode *rootElem = NULL;
    SVG * newSvg = NULL;

    newSvg = malloc(sizeof(SVG)); // mallocs space for SVG, elements, and it's lists
    newSvg = initializeSVG(newSvg); // initializes SVG's elements and lists
    rootElem = xmlDocGetRootElement(doc); // gets the root element (svg) in the xml doc
    strcpy(newSvg->namespace, (char*)rootElem->ns->href); // stores the namespace in the SVG struct

    newSvg = traverseTree(rootElem, newSvg, NULL); // traverses the xml file, populating newSvg's lists and elements

    return newSvg;
}

SVG * traverseTree(xmlNode * node, SVG * newSvg, Group * svgGroup) //traverses through the SVG file, calls itself on child elements or when it encounters groups
{
    xmlNode *currentElem = NULL; 
    int calledInGroup = 0;

    for(currentElem = node; currentElem; currentElem = currentElem->next) // loops through the tree while the node is not NULL
    {
        if(currentElem->type == XML_ELEMENT_NODE)
        {
            if(!strcmp((char*)currentElem->name, "svg")) // if the current node tag is svg
            {
                xmlAttr * currAttr = currentElem->properties;

                while(currAttr != NULL) // while there are still attributes in svg
                {    
                    Attribute *newAttribute = malloc(sizeof(Attribute) + (strlen((char*)(currAttr->children->content)) * sizeof(char)) + 1); // create a new attribute and allocate correct size
                    createAttribute(newAttribute, (char*)(currAttr->name), (char*)(currAttr->children->content)); // populates the new attribute
                    insertBack(newSvg->otherAttributes, newAttribute); // inserts the attribute into svg's otherAttributes list
                        
                    currAttr = currAttr->next; // cycles to next element
                }
            }
            else if(!strcmp((char*)currentElem->name, "title")) // gets the title of the svg
            {
                strcpy(newSvg->title, (char*)currentElem->children->content);
            }
            else if(!strcmp((char*)currentElem->name, "desc")) // gets the description of the svg
            {
                char tempDesc[256];
                strcpy(tempDesc, (char*)currentElem->children->content);

                for(int i = 0; i < strlen(tempDesc) -1; i++)
                {
                    if(tempDesc[i] == '\n')
                    {
                        tempDesc[i] = ' ';
                    }
                }

                strcpy(newSvg->description, tempDesc);
            }
            else if(!strcmp((char*)currentElem->name, "rect")) // if the node tag is rect
            {
                Rectangle *newRect = malloc(sizeof(Rectangle)); // creates new rectangle
                strcpy(newRect->units, "\0");

                newRect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes); // creates the new rectangles otherAttributes list

                xmlAttr * currAttr = currentElem->properties;
                while(currAttr != NULL) // loops through rect's attributes and populates the new rectangle with the correct attributes values
                {    
                    if(!strcmp((char*)(currAttr->name), "x"))
                    {
                        newRect->x = checkForUnits((char*)currAttr->children->content, newRect->units);
                    }
                    else if(!strcmp((char*)(currAttr->name), "y"))
                    {
                        newRect->y = checkForUnits((char*)currAttr->children->content, newRect->units);
                    }
                    else if(!strcmp((char*)(currAttr->name), "width"))
                    {
                        newRect->width = checkForUnits((char*)currAttr->children->content, newRect->units);
                    }
                    else if(!strcmp((char*)(currAttr->name), "height"))
                    {
                        newRect->height = checkForUnits((char*)currAttr->children->content, newRect->units);
                    }
                    else // adds the current attribute to the new rects otherAttributes list
                    {
                        Attribute *newAttribute = malloc(sizeof(Attribute) + (strlen((char*)(currAttr->children->content)) * sizeof(char)) + 1);
                        createAttribute(newAttribute, (char*)(currAttr->name), (char*)(currAttr->children->content));
                        insertBack(newRect->otherAttributes, newAttribute);
                    }
                    currAttr = currAttr->next;
                }

                if(svgGroup == NULL) // if the country is not being called inside a group
                {
                    insertBack(newSvg->rectangles, newRect); //insert rect into the SVG
                }
                else
                {
                    insertBack(svgGroup->rectangles, newRect); // insert rect into the calling group
                }

            }
            else if(!strcmp((char*)currentElem->name, "circle")) // creates new circle
            {
                Circle * newCirc = malloc(sizeof(Circle));
                strcpy(newCirc->units, "\0");

                newCirc->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes); // creates the new circle otherAttributes list

                xmlAttr * currAttr = currentElem->properties; 
                while(currAttr != NULL) // loops through circle's attributes and populates the new circle with the correct attributes values
                {
                    if(!strcmp((char*)(currAttr->name), "cx"))
                    {
                        newCirc->cx = checkForUnits((char*)currAttr->children->content, newCirc->units);
                    }
                    else if(!strcmp((char*)(currAttr->name), "cy"))
                    {
                        newCirc->cy = checkForUnits((char*)currAttr->children->content, newCirc->units);
                    }
                    else if(!strcmp((char*)(currAttr->name), "r"))
                    {
                        newCirc->r = checkForUnits((char*)currAttr->children->content, newCirc->units);
                    }
                    else // adds the current attribute to the new circle otherAttributes list
                    {
                        Attribute *newAttribute = malloc(sizeof(Attribute) + (strlen((char*)(currAttr->children->content)) * sizeof(char)) + 1);
                        createAttribute(newAttribute, (char*)(currAttr->name), (char*)(currAttr->children->content));
                        insertBack(newCirc->otherAttributes, newAttribute);
                    }
                    currAttr = currAttr->next;
                }

                if(svgGroup == NULL) // if the country is not being called inside a group
                {
                    insertBack(newSvg->circles, newCirc); // insert circle into the SVG
                }
                else
                {
                    insertBack(svgGroup->circles, newCirc); // insert circ into the calling group
                }

            }
            else if(!strcmp((char*)currentElem->name, "g")) // if the current node tag is g (group)
            {
                xmlAttr * currAttr = currentElem->properties;
                Group * newGroup = malloc(sizeof(Group)); // creates a new group

                //block creates the lists inside the new group
                newGroup->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
                newGroup->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);
                newGroup->circles = initializeList(circleToString, deleteCircle, compareCircles);
                newGroup->paths = initializeList(pathToString, deletePath, comparePaths);
                newGroup->groups = initializeList(groupToString, deleteGroup, compareGroups);
                
                while(currAttr != NULL) // gets the attributes of the new group and stores then in the otherAttributes list
                {
                    Attribute *newAttribute = malloc(sizeof(Attribute) + (strlen((char*)(currAttr->children->content)) * sizeof(char)) + 1);
                    createAttribute(newAttribute, (char*)(currAttr->name), (char*)(currAttr->children->content));
                    insertBack(newGroup->otherAttributes, newAttribute);
                    currAttr = currAttr->next;
                }

                newSvg = traverseTree(currentElem->children, newSvg, newGroup); // recursivesly calls the traverseTree function on itself

                if(svgGroup == NULL) // if the function is not being called from inside a group
                {
                    insertBack(newSvg->groups, newGroup);
                }
                else // if the function is being callled from inside a group
                {
                    insertBack(svgGroup->groups, newGroup);
                }

                calledInGroup = 1;
            }   
            else if(!strcmp((char*)currentElem->name, "path")) // if the current nodes tag is path
            {
                xmlAttr * currAttr = currentElem->properties;
                Path * newPath = malloc(sizeof(Path));

                newPath->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes); // creates newPath's otherAttribute list

                while(currAttr != NULL)
                {
                    if(!strcmp((char*)(currAttr->name), "d")) // if the attribute is data
                    {
                        newPath = realloc(newPath, sizeof(Path) + (strlen((char*)currAttr->children->content) * sizeof(char) + 1));
                        strcpy(newPath->data, (char*)currAttr->children->content);
                        strcat(newPath->data, "\0");
                    }
                    else // populates newPath's otherAttributes list with the current paths extra attributes
                    {
                        Attribute *newAttribute = malloc(sizeof(Attribute) + (strlen((char*)(currAttr->children->content)) * sizeof(char)) + 1);
                        createAttribute(newAttribute, (char*)(currAttr->name), (char*)(currAttr->children->content));
                        insertBack(newPath->otherAttributes, newAttribute);
                    }
                    currAttr = currAttr->next;
                }

                if(svgGroup == NULL) // if not being called in a group, insert newPath into the svg's path list
                {
                    insertBack(newSvg->paths, newPath);
                }
                else // if being called from inside a list, insert newPath into the calling groups path list
                {
                    insertBack(svgGroup->paths, newPath);
                }
            }
        }

        if(!calledInGroup) // if the function has already called itself on a group, dont call itself again
        {
            traverseTree(currentElem->children, newSvg, NULL);
        }
    }

    return newSvg;
}

float checkForUnits(char * content, char * units)
{
    int flonum = 0; // amount of numbers
    int uninum = 0; // amount of characters that are units
    char * flo = malloc(sizeof(char)); // will hold the float number

    if(strlen(units) == 0) // units array that may or may not be filled (if units is empty)
    {
        for(int i = 0; i < strlen(content); i++) // while i is less than the length of content + 1
        {
            if(isdigit(content[i]) || content[i] == '.' || content[i] == '-') // if content is a digit
            {
                flo[flonum++] = content[i]; // stores digit in flonum and increments flonum++
                flo = realloc(flo, sizeof(char) * flonum + 2); // makes room for next number
            }
            else // if content is not a digit
            {
                units[uninum++] = content[i]; // adds the char to the units array and increments uninum++
            }
        }

        units[uninum] = '\0'; // adds the \0 to the end of units

    }
    else // if units array is already full
    {
        for(int i = 0; i < strlen(content); i++) // loops through content
        {
            if(isdigit(content[i]) || content[i] == '.' || content[i] == '-') // if content is a number
            {
                flo[flonum++] = content[i]; 
                flo = realloc(flo, sizeof(char) * flonum + 2);
            }
        }
    }

    flo[flonum] = '\0'; // adds the \0 to the end of float

    float ret = atof(flo); // converts string flo to a float
    free(flo);

    return ret;
}

char * catStringNFree(char * orgString, char * newString, int freeString) // used to join two malloced strings and then free the no longer needed string
{
    if(orgString == NULL || newString == NULL) // checks if either strings are NULL
    {
        return NULL;
    }

    orgString = realloc(orgString, sizeof(char) * (strlen(orgString) + strlen(newString)) + 1); // reallocates the returning string to accomodate the second strings size

    strcat(orgString, newString); // concatenates the two strings together
    if(freeString == 1) // if the freeString flag is set
    {
        free(newString); // free the no longer needed string
    }
    return orgString;
}

List * searchGroups(List *groups, List * masterList, int rec, int circ, int path, int group) // used to search through all the groups of svg and retrieve the specified elements
{
    ListIterator iter = createIterator(groups);
    Group *currGroup = (Group*)nextElement(&iter);

    while(currGroup != NULL) //more groups in current level
    {
        if(getLength(currGroup->groups) != 0) // currGroup has child groups
        {
            masterList = searchGroups(currGroup->groups, masterList, rec, circ, path, group);
        }

        if(rec == 1) // adds all the rectangles in the current group to the masterList
        {
            ListIterator elemIter = createIterator(currGroup->rectangles);
            Rectangle * rect = (Rectangle*)nextElement(&elemIter);
            while(rect != NULL)
            {
                insertFront(masterList, rect);
                rect = (Rectangle*)nextElement(&elemIter);
            }
        }
        else if (circ == 1) // adds all the circles in the current group to the masterList
        {
            ListIterator elemIter = createIterator(currGroup->circles);
            Circle * circ = (Circle*)nextElement(&elemIter);
            while(circ != NULL)
            {
                insertFront(masterList, circ);
                circ = (Circle*)nextElement(&elemIter);
            }
        }
        else if (path == 1) // adds all the paths in the current group to the masterList
        {
            ListIterator elemIter = createIterator(currGroup->paths);
            Path * path = (Path*)nextElement(&elemIter);
            while(path != NULL)
            {
                insertFront(masterList, path);
                path = (Path*)nextElement(&elemIter);
            }
        }
        else if(group == 1) // adds all the groups in the current group to the masterList
        {
            ListIterator elemIter = createIterator(currGroup->groups);
            Group * group = (Group*)nextElement(&elemIter);
            while(group != NULL)
            {
                insertFront(masterList, group);
                group = (Group*)nextElement(&elemIter);
            }
        }
        currGroup = (Group*)nextElement(&iter); // moves to the next group in the passed in groupList
    }
    return masterList;
}

void traverseGroupsForLen(List* Groups, int* matches, int len) // traverses through all the groups in the passed in list and returns the total number of groups with the same amount of elements specifed by len
{
    ListIterator iter = createIterator(Groups);

    for(Group * currGroup = (Group*)nextElement(&iter); currGroup != NULL; currGroup = (Group*)nextElement(&iter)) // iterates through the passed in group list
    {
        if(getLength(currGroup->groups) != 0) // if there is a child group
        {
            traverseGroupsForLen(currGroup->groups, matches, len);
        }

        int totalLength = getLength(currGroup->rectangles) + getLength(currGroup->circles) + getLength(currGroup->paths) + getLength(currGroup->groups); // adds number of elements in currGroup to totalLength

        if(totalLength == len) // if the totalLength equals len
        {
            (*matches)++; // adds 1 to matches
        }
    }
}

void traverseGroupsForAttrs(List * groups, int * totalAttrs) // traverses the passed in list of groups to find the number of attributes across the svg
{
    ListIterator iter = createIterator(groups);
    Group *currGroup = (Group*)nextElement(&iter);

    while(currGroup != NULL) //more groups in current level
    {
        if(getLength(currGroup->groups) != 0) // currGroup has child groups
        {
            traverseGroupsForAttrs(currGroup->groups, totalAttrs); // calls itself on the child groups
        }

        if(getLength(currGroup->otherAttributes) > 0)
        {
            (*totalAttrs) += getLength(currGroup->otherAttributes); // gets the number of attributes assigned to the current group
        }

        if(getLength(currGroup->rectangles) > 0) // creates a loop that loops through all of the rectangles in currGroup and sums all of there attributes
        {
            ListIterator elemIter = createIterator(currGroup->rectangles);
            Rectangle * rect = (Rectangle*)nextElement(&elemIter);
            while(rect != NULL)
            {
                (*totalAttrs) += getLength(rect->otherAttributes); 
                rect = (Rectangle*)nextElement(&elemIter);
            }
        }
        if (getLength(currGroup->circles) > 0) // creates a loop that loops through all of the circles in currGroup and sums all of there attributes
        {
            ListIterator elemIter = createIterator(currGroup->circles);
            Circle * circ = (Circle*)nextElement(&elemIter);
            while(circ != NULL)
            {
                (*totalAttrs) += getLength(circ->otherAttributes);
                circ = (Circle*)nextElement(&elemIter);
            }
        }
        if (getLength(currGroup->paths) > 0) // creates a loop that loops through all of the paths in currGroup and sums all of there attributes
        {
            ListIterator elemIter = createIterator(currGroup->paths);
            Path * path = (Path*)nextElement(&elemIter);
            while(path != NULL)
            {
                (*totalAttrs) += getLength(path->otherAttributes);
                path = (Path*)nextElement(&elemIter);
            }
        }
        currGroup = (Group*)nextElement(&iter);
    }
}

void dummyDelete() // passed into the lists created by the get... functions
{
    return;
}

bool checkForNegatives(const SVG * img) // function checks rects and circles for negative numbers
{
    if(img == NULL)
    {
        return false;
    }
    //checks svg lists
    if(img->rectangles == NULL || img->circles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL || strlen(img->namespace) == 0)
    {
        return false;
    }
    ListIterator attrIter = createIterator(img->otherAttributes);
    for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
    {
        if(attr->name == NULL)
        {
            return false;
        }
    }
    //rectangles
    List * masterList = getRects(img);
    ListIterator rectIter = createIterator(masterList);
    for(Rectangle * tempRect = nextElement(&rectIter); tempRect != NULL; tempRect = nextElement(&rectIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
    {
        if(tempRect->width < 0 || tempRect->height < 0 || tempRect->units == NULL || tempRect->otherAttributes == NULL)
        {
            freeList(masterList);
            return false;
        }
        ListIterator attrIter = createIterator(tempRect->otherAttributes);
        for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
        {
            if(attr->name == NULL)
            {
                freeList(masterList);
                return false;
            }
        }
    }
    freeList(masterList);
    //circles
    masterList = getCircles(img);
    ListIterator circIter = createIterator(masterList);
    for(Circle * tempCirc = nextElement(&circIter); tempCirc != NULL; tempCirc = nextElement(&circIter)) // loops through all rectangles in the SVG struct and checks if the r values are >= 0
    {
        if(tempCirc->r < 0 || tempCirc->units == NULL || tempCirc->otherAttributes == NULL)
        {
            freeList(masterList);
            return false;
        }
        ListIterator attrIter = createIterator(tempCirc->otherAttributes);
        for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
        {
            if(attr->name == NULL)
            {
                freeList(masterList);
                return false;
            }
        }
    }
    freeList(masterList);
    //paths
    masterList = getPaths(img);
    ListIterator pathIter = createIterator(masterList);
    for(Path * tempPath = nextElement(&pathIter); tempPath != NULL; tempPath = nextElement(&pathIter)) // loops through all rectangles in the SVG struct and checks if the r values are >= 0
    {
        if(tempPath->data == NULL || tempPath->otherAttributes == NULL)
        {
            freeList(masterList);
            return false;
        }
        ListIterator attrIter = createIterator(tempPath->otherAttributes);
        for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
        {
            if(attr->name == NULL)
            {
                freeList(masterList);
                return false;
            }
        }
    }
    freeList(masterList);
    //groups
    masterList = getGroups(img);
    ListIterator groupIter = createIterator(masterList);
    for(Group * tempGroup = nextElement(&groupIter); tempGroup != NULL; tempGroup = nextElement(&groupIter)) // loops through all rectangles in the SVG struct and checks if the r values are >= 0
    {
        if(img->rectangles == NULL || img->circles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL)
        {
            freeList(masterList);
            return false;
        }
        ListIterator attrIter = createIterator(tempGroup->otherAttributes);
        for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loops through all rectangles in the SVG struct and checks if the width and height values are >= 0
        {
            if(attr->name == NULL)
            {
                freeList(masterList);
                return false;
            }
        }
    }
    freeList(masterList);
    
    return true;
}

xmlDoc * svgToXmlDoc(const SVG * svg, xmlDoc * doc, xmlNode * svgGroup, Group * callingGroup) // writes an SVG struct onto an xml doc
{
    xmlNode * rootNode = NULL;
    if(doc == NULL && svgGroup == NULL) // first call of function
    {
        doc = xmlNewDoc(BAD_CAST "1.0");
        rootNode = xmlNewNode(NULL, BAD_CAST "svg");
        xmlDocSetRootElement(doc, rootNode);
        xmlNsPtr ns = xmlNewNs(rootNode, (xmlChar*)svg->namespace, NULL); // creates the namespace
        xmlSetNs(rootNode, ns); // attaches the namespace to the svg node
        ListIterator attrIter = createIterator(svg->otherAttributes);
        for(Attribute * attr = (Attribute*)nextElement(&attrIter); attr != NULL; attr = (Attribute*)nextElement(&attrIter)) // loops through the svg stucts attributes and adds them to the svg node 
        {
            char *nameBuf = malloc(sizeof(char) * strlen(attr->name) + 1);
            char *valueBuf = malloc(sizeof(char) * strlen(attr->value) + 1);
            sprintf(nameBuf, "%s", attr->name);
            sprintf(valueBuf, "%s", attr->value);
            xmlNewProp(rootNode, (xmlChar*)nameBuf, (xmlChar*)valueBuf); // adds the name and value of the attribute to the new attribute
            free(nameBuf);
            free(valueBuf);
        }
        if(strlen(svg->title) != 0) // if there is a title in svg
        {
            xmlNode *currNode = xmlNewChild(rootNode, NULL, BAD_CAST "title", NULL);
            xmlNodeAddContent(currNode, (xmlChar*)svg->title);
        }
        if(strlen(svg->description) != 0) // if there is a description in svg
        {
            xmlNode *currNode = xmlNewChild(rootNode, NULL, BAD_CAST "desc", NULL);
            xmlNodeAddContent(currNode, (xmlChar*)svg->description);
        }
        
    }

    // loops for making the rects
    ListIterator rectIter;
    if(svgGroup == NULL) // if not being called in a group
    {
        rectIter = createIterator(svg->rectangles); // look through svg's rectangles
    }
    else
    {
        rectIter = createIterator(callingGroup->rectangles); // look through the calling groups rectangles
    }        
    for(Rectangle * rect = (Rectangle*)nextElement(&rectIter); rect != NULL; rect = (Rectangle*)nextElement(&rectIter)) // loops through the list of rectangles
    {
        xmlNode* currNode;
        if(svgGroup == NULL) // if not being called in a group
        {
            currNode = xmlNewChild(rootNode, NULL, BAD_CAST "rect", NULL); // adds the rectangle as a child of the svg root node
        }
        else
        {
            currNode = xmlNewChild(svgGroup, NULL, BAD_CAST "rect", NULL); // adds the rectangle as a child of the calling group node
        }
        //adds the x, y, width, and height values to the rect node
        ListIterator attrIter = createIterator(rect->otherAttributes);
        char buffer[100];
        sprintf(buffer, "%f%s", rect->x, rect->units);
        xmlNewProp(currNode, (xmlChar*)"x", (xmlChar*)buffer);
        sprintf(buffer, "%f%s", rect->y, rect->units);
        xmlNewProp(currNode, (xmlChar*)"y", (xmlChar*)buffer);
        sprintf(buffer, "%f%s", rect->width, rect->units);
        xmlNewProp(currNode, (xmlChar*)"width", (xmlChar*)buffer);
        sprintf(buffer, "%f%s", rect->height, rect->units);
        xmlNewProp(currNode, (xmlChar*)"height", (xmlChar*)buffer);
        for(Attribute * attr = (Attribute*)nextElement(&attrIter); attr != NULL; attr = (Attribute*)nextElement(&attrIter)) // adds the rects attributes to the rect node
        {
            char *nameBuf = malloc(sizeof(char) * strlen(attr->name) + 1);
            char *valueBuf = malloc(sizeof(char) * strlen(attr->value) + 1);
            sprintf(nameBuf, "%s", attr->name);
            sprintf(valueBuf, "%s", attr->value);
            xmlNewProp(currNode, (xmlChar*)nameBuf, (xmlChar*)valueBuf);
            free(nameBuf);
            free(valueBuf);
        }
    }
    //loops for making the circles
    ListIterator circIter;
    if(svgGroup == NULL) // if not being called in a group
    {
        circIter = createIterator(svg->circles); // look through svg's circles
    }
    else
    {
        circIter = createIterator(callingGroup->circles);  // look through the calling groups circles
    } 
    for(Circle * circ = (Circle*)nextElement(&circIter); circ != NULL; circ = (Circle*)nextElement(&circIter)) // loops through the list of circles
    {
        xmlNode* currNode;
        if(svgGroup == NULL)
        {
            currNode = xmlNewChild(rootNode, NULL, BAD_CAST "circle", NULL); // adds the circle as a child of the svg root node
        }
        else
        {
            currNode = xmlNewChild(svgGroup, NULL, BAD_CAST "circle", NULL); // adds the circle as a child of the calling group node
        }
        //adds the cx, cy, and r values to the circle node
        ListIterator attrIter = createIterator(circ->otherAttributes);
        char buffer[100];
        sprintf(buffer, "%f%s", circ->cx, circ->units);
        xmlNewProp(currNode, (xmlChar*)"cx", (xmlChar*)buffer);
        sprintf(buffer, "%f%s", circ->cy, circ->units);
        xmlNewProp(currNode, (xmlChar*)"cy", (xmlChar*)buffer);
        sprintf(buffer, "%f%s", circ->r, circ->units);
        xmlNewProp(currNode, (xmlChar*)"r", (xmlChar*)buffer);;
        for(Attribute * attr = (Attribute*)nextElement(&attrIter); attr != NULL; attr = (Attribute*)nextElement(&attrIter)) // adds the circles attributes to the circle node
        {
            char *nameBuf = malloc(sizeof(char) * strlen(attr->name) + 1);
            char *valueBuf = malloc(sizeof(char) * strlen(attr->value) + 1);
            sprintf(nameBuf, "%s", attr->name);
            sprintf(valueBuf, "%s", attr->value);
            xmlNewProp(currNode, (xmlChar*)nameBuf, (xmlChar*)valueBuf);
            free(nameBuf);
            free(valueBuf);;
        }
    }
    //loops for making the paths
    ListIterator pathIter;
    if(svgGroup == NULL) // if not being called inside a group
    {
        pathIter = createIterator(svg->paths); // look throught the svg's paths
    }
    else
    {
        pathIter = createIterator(callingGroup->paths); // look through the calling groups paths
    } 
    for(Path * path = (Path*)nextElement(&pathIter); path != NULL; path = (Path*)nextElement(&pathIter)) // loops through the list of paths
    {
        xmlNode* currNode;
        if(svgGroup == NULL)
        {
            currNode = xmlNewChild(rootNode, NULL, BAD_CAST "path", NULL); // adds the path as a child of the svg node
        }
        else
        {
            currNode = xmlNewChild(svgGroup, NULL, BAD_CAST "path", NULL); // adds the path as a child of the calling group node
        }
        // adds the d value to the path node
        ListIterator attrIter = createIterator(path->otherAttributes);
        char * buffer = malloc(sizeof(char) * (strlen(path->data) + 1));
        sprintf(buffer, "%s", path->data);
        xmlNewProp(currNode, (xmlChar*)"d", (xmlChar*)buffer);
        for(Attribute * attr = (Attribute*)nextElement(&attrIter); attr != NULL; attr = (Attribute*)nextElement(&attrIter)) // loop adds the paths attributes to the path node
        {
            char *nameBuf = malloc(sizeof(char) * strlen(attr->name) + 1);
            char *valueBuf = malloc(sizeof(char) * strlen(attr->value) + 1);
            sprintf(nameBuf, "%s", attr->name);
            sprintf(valueBuf, "%s", attr->value);
            xmlNewProp(currNode, (xmlChar*)nameBuf, (xmlChar*)valueBuf);
            free(nameBuf);
            free(valueBuf);
        }
        free(buffer);
    }
    //loops for making the groups
    ListIterator groupIter;
    if(svgGroup == NULL) // if not being called inside a group
    {
        groupIter = createIterator(svg->groups); // looks through the svg's groups
    }
    else
    {
        groupIter = createIterator(callingGroup->groups); // looks through the calling group's groups
    } 
    for(Group * group = (Group*)nextElement(&groupIter); group != NULL; group = (Group*)nextElement(&groupIter)) // loops through the list if groups
    {
        xmlNode* currNode;
        if(svgGroup == NULL) // if not being called inside a group
        {
            currNode = xmlNewChild(rootNode, NULL, BAD_CAST "g", NULL); // creates a child of the svg node
        }
        else
        {
            currNode = xmlNewChild(svgGroup, NULL, BAD_CAST "g", NULL); // creates a child of the calling group
        }
        ListIterator attrIter = createIterator(group->otherAttributes);
        for(Attribute * attr = (Attribute*)nextElement(&attrIter); attr != NULL; attr = (Attribute*)nextElement(&attrIter)) // loop adds the groups attributes to the group node
        {
            char *nameBuf = malloc(sizeof(char) * strlen(attr->name) + 1);
            char *valueBuf = malloc(sizeof(char) * strlen(attr->value) + 1);
            sprintf(nameBuf, "%s", attr->name);
            sprintf(valueBuf, "%s", attr->value);
            xmlNewProp(currNode, (xmlChar*)nameBuf, (xmlChar*)valueBuf);
            free(nameBuf);
            free(valueBuf);
        }

        doc = svgToXmlDoc(svg, doc, currNode, group); // calls the function again inside of the current group
    }

    xmlCleanupParser();
    return doc;
}

bool validateXmlTree(xmlDoc * doc, const char * schemaFile) // used to validate and xml doc using a schemafile
{
    xmlSchema *schema = NULL;

    xmlSchemaParserCtxtPtr schemaParser;
    schemaParser = xmlSchemaNewParserCtxt(schemaFile); // creates a new schema file parser
    schema = xmlSchemaParse(schemaParser); // creates a schema that xml files will follow
    xmlSchemaFreeParserCtxt(schemaParser);

    xmlSchemaValidCtxtPtr schemaValid;
    schemaValid = xmlSchemaNewValidCtxt(schema); // creates a valid schema pointer

    if(xmlSchemaValidateDoc(schemaValid, doc) != 0) // validates the doc against the schemafile, if it fails 
    {
        if(schema != NULL)
        {
            xmlSchemaFree(schema);
        }
        xmlSchemaFreeValidCtxt(schemaValid);
        return false;
    }
    else // if the validation succeeds
    {
        if(schema != NULL)
        {
            xmlSchemaFree(schema);
        }
        xmlSchemaFreeValidCtxt(schemaValid);
        return true;
    }
}
  