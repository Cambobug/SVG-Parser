#include "SVGHelper.h"

SVG* createSVG(const char * fileName)
{
    xmlDoc *doc = NULL;
    xmlNode *rootElem = NULL;

    doc = xmlReadFile(fileName, NULL, 0); //attempts to open xml file

    if(doc == NULL) // if doc returns NULL, createSVG will return NULL
    {
        printf("Error: file does not exist");
        return NULL;
    }

    SVG* newSvg = malloc(sizeof(SVG)); // mallocs space for SVG, elements, and it's lists

    newSvg = initializeSVG(newSvg); // initializes SVG's elements and lists

    rootElem = xmlDocGetRootElement(doc); // gets the root element (svg) in the xml doc

    strcpy(newSvg->namespace, (char*)rootElem->ns->href); // stores the namespace in the SVG struct

    traverseTree(rootElem, newSvg, NULL); // traverses the xml file, populating newSvg's lists and elements

    xmlFreeDoc(doc); 
    xmlCleanupParser();
    return newSvg;
}

void deleteSVG(SVG* img) // deallocates the svg and all of its subcomponents 
{
    freeList(img->otherAttributes);
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);

    free(img);
}

char* SVGToString(const SVG* img) // prints all of the information from the SVG img onto the terminal
{
    char * finalStr = malloc(sizeof(char) * (75 + strlen(img->namespace) + strlen(img->title) + strlen(img->description))); 
    // mallocs enough space for the namespace, title, and description, as well as chars needed in sprintf
    sprintf(finalStr, "Namespace: %s | Title: %s\n Description: %s\n", img->namespace, img->title, img->description); // 
    finalStr = catStringNFree(finalStr, toString(img->otherAttributes), 1); // takes finalStr, strcats the second argument to finalStr, then frees the second argument
    finalStr = catStringNFree(finalStr, toString(img->rectangles), 1);
    finalStr = catStringNFree(finalStr, toString(img->circles), 1);
    finalStr = catStringNFree(finalStr, toString(img->paths), 1);
    finalStr = catStringNFree(finalStr, toString(img->groups), 1);

    return finalStr;
}

/*
    List Delete Functions
*/
void deleteAttribute( void* data) // deletes attribute
{
    if(data == NULL)
    {
        return;
    }
    free(((Attribute*)data)->name); // frees the attribute name
    free(((Attribute*)data)); //frees the attribute struct and flex array
}

void deleteGroup(void* data) // deletes a group
{       
    if(data == NULL) 
    {
        return;
    }
    //frees lists inside of group
    freeList(((Group*)data)->groups);
    freeList(((Group*)data)->paths);
    freeList(((Group*)data)->circles);
    freeList(((Group*)data)->rectangles);
    freeList(((Group*)data)->otherAttributes);
    free(((Group*)data)); // frees group
}

void deleteRectangle(void* data) // deletes a rectangle
{
    if(data == NULL)
    {
        return;
    }
    freeList(((Rectangle*)data)->otherAttributes);
    free(((Rectangle*)data));
}

void deleteCircle(void* data) // deletes a circle
{
    if(data == NULL)
    {
        return;
    }
    freeList(((Circle*)data)->otherAttributes);
    free(((Circle*)data));
}

void deletePath(void* data) //deletes a path
{
    if(data == NULL)
    {
        return;
    }
    freeList(((Path*)data)->otherAttributes);
    free(((Path*)data));
}

/*
    List toString Functions
*/

char* attributeToString(void* data) // returns an attribute as a string
{
    if(data == NULL)
    {
        return NULL;
    }
    char * atStr = malloc(sizeof(char) * (35 + strlen(((Attribute*)data)->name) + strlen(((Attribute*)data)->value))); // allocates space for the attribute string
    sprintf(atStr, "Attribute Name: %s Attribute Value: %s", ((Attribute*)data)->name, ((Attribute*)data)->value); // creates a formatted string with arguments and stores it in atStr
    return atStr;
}

char* groupToString(void* data) // returns a group as a string
{
    if(data == NULL)
    {
        return NULL;
    }
    //gets a string representing each list in the group
    char * atStr = toString(((Group*)data)->otherAttributes);
    char * recStr = toString(((Group*)data)->rectangles);
    char * cirStr = toString(((Group*)data)->circles);
    char * pathStr = toString(((Group*)data)->paths);
    char * groupStr = toString(((Group*)data)->groups);

    char * str = malloc(sizeof(char) * (75 + strlen(atStr) + strlen(cirStr) + strlen(recStr) + strlen(pathStr) + strlen(groupStr))); // allocates space for group string

    sprintf(str, "---Attributes---%s\n---Rectangles---%s\n---Circles---%s\n---Paths---%s\n---Groups---%s\n", atStr, recStr, cirStr, pathStr, groupStr);

    free(atStr);
    free(recStr);
    free(cirStr);
    free(pathStr);
    free(groupStr);

    return str;
}

char* rectangleToString(void* data) // prints out a rectangle as a string
{
    if(data == NULL)
    {
        return NULL;
    }
    char * atStr = toString(((Rectangle*)data)->otherAttributes); // gets the rectangles attributes
    char * str = malloc(sizeof(char) * (75 + strlen(atStr) + strlen(((Rectangle*)data)->units) + (sizeof(float) * 4)));
    sprintf(str, "x: %f, y: %f, width: %f, height: %f, Units: %s", ((Rectangle*)data)->x, ((Rectangle*)data)->y, ((Rectangle*)data)->width, ((Rectangle*)data)->height, ((Rectangle*)data)->units);
    strcat(str, atStr); // adds the rectangles attributes to the end of the rectangle string
    free(atStr);
    return str;
}

char* circleToString(void* data) // prints out a circle as a string
{
    if(data == NULL)
    {
        return NULL;
    }
    char * atStr = toString(((Circle*)data)->otherAttributes); // gets the circles attributes
    char * str = malloc(sizeof(char) * (75 + strlen(atStr) + strlen(((Circle*)data)->units)) + (sizeof(float) * 3));
    sprintf(str, "cx: %f, cy: %f, r: %f, Units: %s", ((Circle*)data)->cx, ((Circle*)data)->cy, ((Circle*)data)->r, ((Circle*)data)->units);
    strcat(str, atStr); // adds the circles attributes to the end of the rectangle string
    free(atStr);
    return str;
}

char* pathToString(void* data) // prints out a path as a string
{
    if(data == NULL)
    {
        return NULL;
    }
    char * atStr = toString(((Path*)data)->otherAttributes); // gets the paths attributes
    char * str = malloc(sizeof(char) * (75 + strlen(atStr) + strlen(((Path*)data)->data)));
    sprintf(str, "d: %s", ((Path*)data)->data);
    strcat(str, atStr); // adds the paths attributes to the end of the rectangle string
    free(atStr);
    return str;
}

/* 
    Dummy Compare Functions
*/

int compareAttributes(const void *first, const void *second)
{
    return 0;
}

int compareGroups(const void *first, const void *second)
{
    return 0;
}

int compareRectangles(const void *first, const void *second)
{
    return 0;
}

int compareCircles(const void *first, const void *second)
{
    return 0;
}

int comparePaths(const void *first, const void *second)
{
    return 0;
}

List* getRects(const SVG* img)
{   
    if(img == NULL)
    {
        return NULL;
    }
    List * masterList = initializeList(rectangleToString, dummyDelete, compareRectangles);

    masterList = searchGroups(img->groups, masterList, 1, 0, 0, 0);

    ListIterator elemIter = createIterator(img->rectangles);
    for(Rectangle * rect = nextElement(&elemIter); rect != NULL; rect = nextElement(&elemIter))
    {
        insertBack(masterList, rect);
    }

    return masterList;
}

List* getCircles(const SVG* img)
{
    if(img == NULL)
    {
        return NULL;
    }
    List * masterList = initializeList(circleToString, dummyDelete, compareCircles);

    masterList = searchGroups(img->groups, masterList, 0, 1, 0, 0);

    ListIterator elemIter = createIterator(img->circles);
    for(Circle * circ = nextElement(&elemIter); circ != NULL; circ = nextElement(&elemIter))
    {
        insertBack(masterList, circ);
    }

    return masterList;
}

List* getPaths(const SVG* img)
{
    if(img == NULL)
    {
        return NULL;
    }
    List * masterList = initializeList(pathToString, dummyDelete, comparePaths);

    masterList = searchGroups(img->groups, masterList, 0, 0, 1, 0);

    ListIterator elemIter = createIterator(img->paths);
    for(Path * pat = nextElement(&elemIter); pat != NULL; pat = nextElement(&elemIter))
    {
        insertBack(masterList, pat);
    }

    return masterList;
}

List* getGroups(const SVG* img)
{
    if(img == NULL)
    {
        return NULL;
    }
    List * masterList = initializeList(groupToString, dummyDelete, compareGroups);

    masterList = searchGroups(img->groups, masterList, 0, 0, 0, 1);
    
    ListIterator elemIter = createIterator(img->groups);
    for(Group * gro = nextElement(&elemIter); gro != NULL; gro = nextElement(&elemIter))
    {
        insertBack(masterList, gro);
    }
    
    return masterList;
}

int numRectsWithArea(const SVG* img, float area)
{
    if(area < 0 || img == NULL)
    {
        return 0;
    }
    int matches = 0;
    List * rectList = getRects(img);

    ListIterator iter = createIterator(rectList);
    for(Rectangle * rect = nextElement(&iter); rect != NULL; rect = nextElement(&iter))
    {
        if((rect->width * rect->height) == area)
        {
            matches++;
        }
    }
    
    return matches;
}

int numCirclesWithArea(const SVG* img, float area)
{
    if(area < 0 || img == NULL)
    {
        return 0;
    }
    int matches = 0;
    List * circList = getCircles(img);

    ListIterator iter = createIterator(circList);
    for(Circle * circ = nextElement(&iter); circ != NULL; circ = nextElement(&iter))
    {
        if((3.14 * circ->r * circ->r) == area)
        {
            matches++;
        }
    }
    
    return matches;
}

int numPathsWithdata(const SVG* img, const char* data)
{
    if(!strcmp(data, "") || img == NULL)
    {
        return 0;
    }
    int matches = 0;
    List * pathList = getPaths(img);

    ListIterator iter = createIterator(pathList);
    for(Path * pat = nextElement(&iter); pat != NULL; pat = nextElement(&iter))
    {
        if(!strcmp(pat->data, data))
        {
            matches++;
        }
    }

    return matches;
}

int numGroupsWithLen(const SVG* img, int len)
{
    if(img == NULL || len <= 0)
    {
        return 0;
    }
    int matches = 0;

    traverseGroupsForLen(img->groups, &matches, len);

    return matches;
}

int numAttr(const SVG* img)
{
    int numAttrs = 0;

    if(img == NULL)
    {
        return 0;
    }

    numAttrs += getLength(img->otherAttributes);

    traverseGroupsForAttrs(img->groups, &numAttrs);

    if(getLength(img->rectangles) > 0)
    {
        ListIterator iter = createIterator(img->rectangles);
        for(Rectangle * rect = nextElement(&iter); rect != NULL; rect = nextElement(&iter))
        {
            numAttrs += getLength(rect->otherAttributes);
        }
    }
    if (getLength(img->circles) > 0)
    {
        ListIterator iter = createIterator(img->circles);
        for(Circle * circ = nextElement(&iter); circ != NULL; circ = nextElement(&iter))
        {
            numAttrs += getLength(circ->otherAttributes);
        }
    }
    if (getLength(img->paths) > 0)
    {
        ListIterator iter = createIterator(img->paths);
        for(Path * pat = nextElement(&iter); pat != NULL; pat = nextElement(&iter))
        {
            numAttrs += getLength(pat->otherAttributes);
        }
    }

    return numAttrs;
}

SVG* createValidSVG(const char* fileName, const char* schemaFile) // function creates a valid SVG, first by validating the svg file, then by validating the file created by the svg struct
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    xmlDoc *doc = NULL;

    doc = xmlReadFile(fileName, NULL, 0); //attempts to open xml file

    if(doc == NULL) // if doc returns NULL, createSVG will return NULL
    {
        printf("Error: file does not exist\n");
        return NULL;
    }

    bool validDoc = validateXmlTree(doc, schemaFile); // validates the xml file passed into the program

    if(validDoc == false)
    {
        xmlFreeDoc(doc); 
        xmlCleanupParser();
        return NULL;
    }

    SVG * newSvg = createSVGTree(doc); // creates the SVG struct
    xmlFreeDoc(doc); 

    bool validSVG = validateSVG(newSvg, schemaFile); // validates the SVG struct by converting it back to an xml file and validating the xml file

    if(validSVG == false)
    {
        deleteSVG(newSvg); 
        xmlCleanupParser();
        return NULL;
    }

    xmlCleanupParser();
    return newSvg;
}

bool writeSVG(const SVG* img, const char* fileName)
{
    if(img == NULL || fileName == NULL)
    {
        return false;
    }
    xmlDoc * doc = NULL;
    doc = svgToXmlDoc(img, doc, NULL, NULL); // helper function converts the svg struct to an xml doc

    if(doc == NULL)
    {
        xmlCleanupParser();
        return false;
    }

    int result = xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1); // prints the xml doc into the filename specified in the command line
    if(result == -1)
    {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return true;
}

bool validateSVG(const SVG* img, const char* schemaFile)
{
    if(img == NULL || schemaFile == NULL)
    {
        return false;
    }

    bool negCheck = checkForNegatives(img); // checks the SVG for negative or invalid values

    if(negCheck == false) 
    {
        return negCheck;
    }

    xmlDoc * newDoc = NULL;
    newDoc = svgToXmlDoc(img, newDoc, NULL, NULL); // converts the svg struct to a xml doc 

    bool valid = validateXmlTree(newDoc, schemaFile); // validates the xml doc

    xmlFreeDoc(newDoc);
    return valid;
}

bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) // function edits the value of an already existing attribute in the SVG struct or it create a new attribute
{
    if(img == NULL || newAttribute == NULL || newAttribute->name == NULL)
    {
        return false;
    }

    if(elemType == SVG_IMG) // if the attribute is in svg node
    {
        bool foundAttr = false;
        Node * currNode = img->otherAttributes->head; // sets currNode to head
        if(currNode == NULL)
        {
            return false;
        }
        Attribute *tempAttr;
        while(currNode != NULL) // loops through attributes and tries to find the specified attribute
        {
            tempAttr = currNode->data;
            if(!strcmp(tempAttr->name, newAttribute->name)) // if the attribute is found
            {
                tempAttr = realloc(tempAttr, sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
                strcpy(tempAttr->value, newAttribute->value);
                currNode->data = tempAttr;
                foundAttr = true;
                break;
            }
            currNode = currNode->next;
        }

        if(foundAttr == false) // if the attribute has not been found
        {
            //Attribute * newAttrCopy = malloc(sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
            //createAttribute(newAttrCopy, newAttribute->name, newAttribute->value);
            insertBack(img->otherAttributes, newAttribute);
        }
    }
    else if(elemType == RECT && elemIndex >= 0) // if the attribute is in a rectangle
    {   
        ListIterator rectIter = createIterator(img->rectangles);
        Rectangle *tempRect = nextElement(&rectIter); 
        if(elemIndex == 0 && tempRect == NULL)
        {
            return false;
        }
        for(int i = 0; i < elemIndex; i++) // loop finds specified rect
        {
            if(tempRect == NULL)
            {
                return false;
            }
            tempRect = nextElement(&rectIter);
        }
        bool foundAttr = false;

        if(!strcmp(newAttribute->name, "x")) // checks x value
        {
            tempRect->x = atof(newAttribute->value);
            foundAttr = true;
        }
        else if(!strcmp(newAttribute->name, "y")) // checks y value
        {
            tempRect->y = atof(newAttribute->value);
            foundAttr = true;
        }
        else if(!strcmp(newAttribute->name, "width")) // checks width value
        {
            if(atof(newAttribute->value)< 0) // makes sure the new value isnt negative
            {
                return false;
            }
            tempRect->width = atof(newAttribute->value);
            foundAttr = true;
        }
        else if(!strcmp(newAttribute->name, "height")) // checks height value
        {
            if(atof(newAttribute->value) < 0) // makes sure the new value isnt negative
            {
                return false;
            }
            tempRect->height = atof(newAttribute->value);
            foundAttr = true;
        }
        else
        {
            Node * currNode = tempRect->otherAttributes->head;
            Attribute *tempAttr;
            while(currNode != NULL) // loops through attributes and tries to find the specified attribute
            {
                tempAttr = currNode->data;
                if(!strcmp(tempAttr->name, newAttribute->name)) // if the attribute is found
                {
                    tempAttr = realloc(tempAttr, sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
                    strcpy(tempAttr->value, newAttribute->value);
                    currNode->data = tempAttr;
                    foundAttr = true;
                    break;
                }
                currNode = currNode->next;
            }
        }
        if(foundAttr == false) // if the attribute is not found
        {
            //Attribute * newAttrCopy = malloc(sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
            //createAttribute(newAttrCopy, newAttribute->name, newAttribute->value);
            insertBack(tempRect->otherAttributes, newAttribute);
        }
    }
    else if(elemType == CIRC && elemIndex >= 0) // if the attribute is in a circle
    {
        ListIterator circIter = createIterator(img->circles);
        Circle *tempCirc = nextElement(&circIter);
        if(elemIndex == 0 && tempCirc == NULL)
        {
            return false;
        } 
        for(int i = 0; i < elemIndex; i++) // finds the specified circle
        {
            if(tempCirc == NULL)
            {
                return false;
            }
            tempCirc = nextElement(&circIter);
        }
        bool foundAttr = false;

        if(!strcmp(newAttribute->name, "cx")) // checks cx value 
        {
            tempCirc->cx = atof(newAttribute->value);
            foundAttr = true;
        }
        else if(!strcmp(newAttribute->name, "cy")) // checks cy value
        {
            tempCirc->cy = atof(newAttribute->value);
            foundAttr = true;
        }
        else if(!strcmp(newAttribute->name, "r")) // checks r value
        {
            if(atof(newAttribute->value) < 0) // checks to see if new value is negative
            {
                return false;
            }
            tempCirc->r = atof(newAttribute->value);
            foundAttr = true;
        }
        else
        {
            Node * currNode = tempCirc->otherAttributes->head;
            Attribute *tempAttr;
            while(currNode != NULL) // loops through attributes and tries to find the specified attribute
            {
                tempAttr = currNode->data;
                if(!strcmp(tempAttr->name, newAttribute->name)) // if the attribute is found
                {
                    tempAttr = realloc(tempAttr, sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
                    strcpy(tempAttr->value, newAttribute->value);
                    currNode->data = tempAttr;
                    foundAttr = true;
                    break;
                }
                currNode = currNode->next;
            }
        }
        if(foundAttr == false)
        {
            //Attribute * newAttrCopy = malloc(sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
            //createAttribute(newAttrCopy, newAttribute->name, newAttribute->value);
            insertBack(tempCirc->otherAttributes, newAttribute);
        }
    }
    else if(elemType == PATH && elemIndex >= 0) // if the attribute is in a path
    {
        Node * currPathNode = img->paths->head;
        if(currPathNode == NULL)
        {
            return false;
        }
        Path *tempPath = currPathNode->data; // finds the specified path
        for(int i = 0; i < elemIndex; i++)
        {
            if(currPathNode == NULL)
            {
                return false;
            }
            currPathNode = currPathNode->next;
            tempPath = currPathNode->data;
        }
        bool foundAttr = false;

        if(!strcmp(newAttribute->name, "d")) // checks d value
        {
            tempPath = realloc(tempPath, sizeof(Path) + (sizeof(char) * strlen(newAttribute->value) + 1));
            strcpy(tempPath->data, newAttribute->value);
            currPathNode->data = tempPath;
            foundAttr = true;
        }
        else
        {
            Node * currNode = tempPath->otherAttributes->head;
            Attribute *tempAttr;
            while(currNode != NULL) // loops through attributes and tries to find the specified attribute
            {
                tempAttr = currNode->data;
                if(!strcmp(tempAttr->name, newAttribute->name)) // if the attribute is found
                {
                    tempAttr = realloc(tempAttr, sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
                    strcpy(tempAttr->value, newAttribute->value);
                    currNode->data = tempAttr;
                    foundAttr = true;
                    break;
                }
                currNode = currNode->next;
            }
        }
        if(foundAttr == false) // if the attribute is not found
        {
           // Attribute * newAttrCopy = malloc(sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
            //createAttribute(newAttrCopy, newAttribute->name, newAttribute->value);
            insertBack(tempPath->otherAttributes, newAttribute);
        }
    }
    else if(elemType == GROUP && elemIndex >= 0) // if the attribute is in a group
    {
        ListIterator groupIter = createIterator(img->groups);
        Group *tempGroup = nextElement(&groupIter); // finds the specified group
        if(elemIndex == 0 && tempGroup == NULL)
        {
            return false;
        }
        for(int i = 0; i < elemIndex; i++)
        {
            if(tempGroup == NULL)
            {
                return false;
            }
            tempGroup = nextElement(&groupIter);
        }
        bool foundAttr = false;
        Node * currNode = tempGroup->otherAttributes->head;
        Attribute *tempAttr;
        while(currNode != NULL) // loops through attributes and tries to find the specified attribute
        {
            tempAttr = currNode->data;
            if(!strcmp(tempAttr->name, newAttribute->name)) // if the attribute is found
            {
                tempAttr = realloc(tempAttr, sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
                strcpy(tempAttr->value, newAttribute->value);
                currNode->data = tempAttr;
                foundAttr = true;
                break;
            }
            currNode = currNode->next;
        }

        if(foundAttr == false) // if the attribute doesnt exist
        {
            //Attribute * newAttrCopy = malloc(sizeof(Attribute) + (sizeof(char) * strlen(newAttribute->value) + 1));
            //createAttribute(newAttrCopy, newAttribute->name, newAttribute->value);
            insertBack(tempGroup->otherAttributes, newAttribute);
        }
    }
    else // if the elemtype is not any of the options above
    {
        return false;
    }

    return true;
}

void addComponent(SVG* img, elementType type, void* newElement) // adds an already created rect, circ or path to the SVG
{
    if(img == NULL || newElement == NULL)
    {
        return;
    }

    if(type == RECT)
    {
        insertBack(img->rectangles, newElement);
    }
    else if(type == CIRC)
    {
        insertBack(img->circles, newElement);
    }
    else if(type == PATH)
    {
        insertBack(img->paths, newElement);    
    }
    else
    {
        return;
    }

    return;
}

char * attrToJSON(const Attribute * a) // converts an attribute to a JSON string
{
    if(a == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    char * ret = malloc(sizeof(char) * (strlen(a->name) + strlen(a->value) + 1) + 50);
    sprintf(ret, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
    return ret;
}

char * circleToJSON(const Circle *c) // converts a circle to a JSON string
{
    if(c == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    int circAttrs = getLength(c->otherAttributes);
    char * ret = malloc(sizeof(char) * (strlen(c->units) + (sizeof(float) * 3) + sizeof(int) + 75));
    sprintf(ret, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, circAttrs, c->units);
    return ret;
}

char * rectToJSON(const Rectangle *r) // converts a rectangle to a JSON string
{
    if(r == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    int rectAttrs = getLength(r->otherAttributes);
    char * ret = malloc(sizeof(char) * (strlen(r->units) + (sizeof(float) * 4) + sizeof(int) + 75));
    sprintf(ret, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, rectAttrs, r->units);
    return ret;
}

char * pathToJSON(const Path * p) // converts a path to a JSON string
{
    if(p == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    int pathAttrs = getLength(p->otherAttributes);
    char * truncPath = malloc(sizeof(char) * 65);
    strncpy(truncPath, p->data, 64);
    truncPath[64] = '\0';
    char * ret = malloc(sizeof(char) * (strlen(truncPath) + sizeof(int) + 75));
    sprintf(ret, "{\"d\":\"%s\",\"numAttr\":%d}", truncPath, pathAttrs);
    return ret;
}

char * groupToJSON(const Group * g) // converts a group to a JSON string
{
    if(g == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    int cVal = getLength(g->rectangles);
    cVal += getLength(g->circles);
    cVal += getLength(g->paths);
    cVal += getLength(g->groups);
    int groupAttrs = getLength(g->otherAttributes);
    char * ret = malloc(sizeof(char) * (sizeof(int) * 5) + 75);
    sprintf(ret, "{\"children\":%d,\"numAttr\":%d}", cVal, groupAttrs);
    return ret;
}

char * SVGtoJSON(const SVG * image) // converts an SVG struct to a JSON string
{
    if(image == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "{}");
        return ret;
    }
    char * ret = malloc(sizeof(char) * (sizeof(int) * 4) + 75);
    List * rectList = getRects(image);
    List * circList = getCircles(image);
    List * pathList = getPaths(image);
    List * groupList = getGroups(image);
    sprintf(ret, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", getLength(rectList), getLength(circList), getLength(pathList), getLength(groupList));
    freeList(rectList);
    freeList(circList);
    freeList(pathList);
    freeList(groupList);
    return ret;
}

char * attrListToJSON(const List * list) // converts a list of attributes to a JSON string
{
    if(getLength((List*)list) == 0 || list == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "[]");
        return ret;
    }
    char * ret = malloc(sizeof(char) * 2);
    strcpy(ret, "[");
    ListIterator attrIter = createIterator((List*)list);
    for(Attribute * attr = nextElement(&attrIter); attr != NULL; attr = nextElement(&attrIter)) // loop strcats all of the strings returned by attToJSON together
    {
        char * attrString = attrToJSON(attr);
        ret = realloc(ret, sizeof(char) * (strlen(ret) + strlen(attrString) + 2));
        strcat(attrString, ",");
        strcat(ret, attrString);
        free(attrString);
    }
    ret = realloc(ret, sizeof(char) * (strlen(ret) + 2));
    ret[strlen(ret) - 1] = ']';
    return ret;
}

char* circListToJSON(const List *list) // converts a list of circles to a JSON string
{
    if(getLength((List*)list) == 0 || list == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "[]");
        return ret;
    }
    char * ret = malloc(sizeof(char) * 2);
    strcpy(ret, "[");
    ListIterator circIter = createIterator((List*)list);
    for(Circle* circ = nextElement(&circIter); circ != NULL; circ = nextElement(&circIter)) // loop strcats all of the strings returned by circToJSON together
    {
        char * circString = circleToJSON(circ);
        ret = realloc(ret, sizeof(char) * (strlen(ret) + strlen(circString) + 2));
        strcat(circString, ",");
        strcat(ret, circString);
        free(circString);
    }
    ret = realloc(ret, sizeof(char) * (strlen(ret) + 2));
    ret[strlen(ret) - 1] = ']';
    return ret;
}

char* rectListToJSON(const List *list) // converts a list of rectangles to a JSON string
{
    if(getLength((List*)list) == 0 || list == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "[]");
        return ret;
    }
    char * ret = malloc(sizeof(char) * 2);
    strcpy(ret, "[");
    ListIterator rectIter = createIterator((List*)list);
    for(Rectangle* rect = nextElement(&rectIter); rect != NULL; rect = nextElement(&rectIter)) // loop strcats all of the strings returned by rectToJSON together
    {
        char * rectString = rectToJSON(rect);
        ret = realloc(ret, sizeof(char) * (strlen(ret) + strlen(rectString) + 2));
        strcat(rectString, ",");
        strcat(ret, rectString);
        free(rectString);
    }
    ret = realloc(ret, sizeof(char) * (strlen(ret) + 2));
    ret[strlen(ret) - 1] = ']';
    return ret;
}

char* pathListToJSON(const List *list) // converts a list of paths to a JSON string
{
    if(getLength((List*)list) == 0 || list == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "[]");
        return ret;
    }
    char * ret = malloc(sizeof(char) * 2);
    strcpy(ret, "[");
    ListIterator pathIter = createIterator((List*)list);
    for(Path* path = nextElement(&pathIter); path != NULL; path = nextElement(&pathIter)) // loop strcats all of the strings returned by pathToJSON together
    {
        char * pathString = pathToJSON(path);
        ret = realloc(ret, sizeof(char) * (strlen(ret) + strlen(pathString) + 2));
        strcat(pathString, ",");
        strcat(ret, pathString);
        free(pathString);
    }
    ret = realloc(ret, sizeof(char) * (strlen(ret) + 2));
    ret[strlen(ret) - 1] = ']';
    return ret;
}

char* groupListToJSON(const List *list) // converts a list of groups to a JSON string
{
    if(getLength((List*)list) == 0 || list == NULL)
    {
        char * ret = malloc(sizeof(char) * 3);
        strcpy(ret, "[]");
        return ret;
    }
    char * ret = malloc(sizeof(char) * 2);
    strcpy(ret, "[");
    ListIterator groupIter = createIterator((List*)list);
    for(Group* group = nextElement(&groupIter); group != NULL; group = nextElement(&groupIter)) // loop strcats all of the strings returned by groupToJSON together
    {
        char * groupString = groupToJSON(group);
        ret = realloc(ret, sizeof(char) * (strlen(ret) + strlen(groupString) + 2));
        strcat(groupString, ",");
        strcat(ret, groupString);
        free(groupString);
    }
    ret = realloc(ret, sizeof(char) * (strlen(ret) + 2));
    ret[strlen(ret) - 1] = ']';
    return ret;
}

SVG* JSONtoSVG(const char* svgString) //converts a JSON string to a an SVG
{
    if(svgString == NULL)
    {
        return NULL;
    }

    SVG * newSvg = malloc(sizeof(SVG)); // mallocs a new svg
    newSvg = initializeSVG(newSvg);
    
    char * titleStr;
    char * descStr;

    char * copy = malloc(sizeof(char) * strlen(svgString) + 1); // copies the svgString
    strcpy(copy, svgString);

    char * currTok = strtok(copy, ",");
    titleStr = malloc(sizeof(char) * strlen(currTok) + 1); // splits the copy in half, and copies the first half to titleStr
    strcpy(titleStr, currTok);
    currTok = strtok(NULL, ",");
    descStr = malloc(sizeof(char) * strlen(currTok) + 1); // copies the other half of copy to descStr
    strcpy(descStr, currTok);

    currTok = strtok(titleStr, "\""); // begins to split titleStr by "
    for(int i = 0; i < 3; i++) // loop splits titleStr until is gets to the content of the title
    {
        currTok = strtok(NULL, "\"");
    }
    char * parsedTitle = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(parsedTitle, currTok); // stores the content of the title in parsedTitle

    currTok = strtok(descStr, "\""); // begins to split descStr by "
    for(int i = 0; i < 2; i++) // loop splits desStr until is gets to the content of the title
    {
        currTok = strtok(NULL, "\"");
    }
    char * parsedDesc = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(parsedDesc, currTok); // stores the content of the description in parsedTitle

    //adds the title, description and namespace to the new svg
    strcpy(newSvg->title, parsedTitle); 
    strcpy(newSvg->description, parsedDesc);
    strcpy(newSvg->namespace, "http://www.w3.org/2000/svg");

    //frees the strings
    free(copy);
    free(titleStr);
    free(descStr);
    free(parsedTitle);
    free(parsedDesc);
    
    return newSvg;
}

Rectangle* JSONtoRect(const char* svgString) // converts a JSON string into a rect
{
    if(svgString == NULL)
    {
        return NULL;
    }

    char *xVal, *yVal, *wVal, *hVal, *units;
    Rectangle * newRect = malloc(sizeof(Rectangle));
    strcpy(newRect->units, "");
    newRect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes); // creates the new rectangles otherAttributes list

    char * copy = malloc(sizeof(char) * strlen(svgString) + 1);
    strcpy(copy, svgString); // copies the svgString into copy

    //splits copy up into separate parts and stores each part in their respective strings
    char * currTok = strtok(copy, ",");
    xVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(xVal, currTok);

    currTok = strtok(NULL, ",");
    yVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(yVal, currTok);

    currTok = strtok(NULL, ",");
    wVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(wVal, currTok);

    currTok = strtok(NULL, ",");
    hVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(hVal, currTok);

    currTok = strtok(NULL, ",");
    units = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(units, currTok);

    //loops through each string and stores the content value in 
    currTok = strtok(xVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newRect->x = atof(currTok);

    currTok = strtok(yVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newRect->y = atof(currTok);

    currTok = strtok(wVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newRect->width = atof(currTok);

    currTok = strtok(hVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newRect->height = atof(currTok);

    currTok = strtok(units, "\"");
    for(int i = 0; i < 2; i++)
    {
        currTok = strtok(NULL, "\"");
    }
    strcpy(newRect->units, currTok);
    
    //frees all of the variables used to store strings
    free(xVal);
    free(yVal);
    free(wVal);
    free(hVal);
    free(units);
    free(copy);

    return newRect;
}

Circle* JSONtoCircle(const char* svgString)
{
    if(svgString == NULL)
    {
        return NULL;
    }
    char *cxVal, *cyVal, *rVal, *units;
    Circle * newCirc = malloc(sizeof(Circle));
    strcpy(newCirc->units, "");
    newCirc->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes); // creates the new circles otherAttributes list

    char * copy = malloc(sizeof(char) * strlen(svgString) + 1);
    strcpy(copy, svgString); // copies the svgString into copy

    //splits copy up into separate parts and stores each part in their respective strings
    char * currTok = strtok(copy, ",");
    cxVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(cxVal, currTok);

    currTok = strtok(NULL, ",");
    cyVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(cyVal, currTok);

    currTok = strtok(NULL, ",");
    rVal = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(rVal, currTok);

    currTok = strtok(NULL, ",");
    units = malloc(sizeof(char) * strlen(currTok) + 1);
    strcpy(units, currTok);

     //loops through each string and stores the content value in
    currTok = strtok(cxVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newCirc->cx = atof(currTok);

    currTok = strtok(cyVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newCirc->cy = atof(currTok);

    currTok = strtok(rVal, ":");
    for(int i = 0; i < 1; i++)
    {
        currTok = strtok(NULL, ":");
    }
    newCirc->r = atof(currTok);

    currTok = strtok(units, "\"");
    for(int i = 0; i < 2; i++)
    {
        currTok = strtok(NULL, "\"");
    }
    strcpy(newCirc->units, currTok);

    //frees the variables used to store strings
    free(cxVal);
    free(cyVal);
    free(rVal);
    free(units);
    free(copy);

    return newCirc;
}

 // checks if the given filename is a valid svg
int isValidSVG(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return 0;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG != NULL) // if a svg has been returned correctly
    {
        deleteSVG(newSVG);
        return 1;
    }
    return 0;
}

// returns an JSON encode of an SVG
char * fileNameToJSON(const char * fileName, const char * schemaFile) 
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * jsonStr = SVGtoJSON(newSVG);

    deleteSVG(newSVG);

    return jsonStr;
}

// returns the title of an SVG using its fileName
char * fileNameToTitle(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * title = malloc(sizeof(char) * (strlen(newSVG->title) + 1));
    strcpy(title, newSVG->title);

    deleteSVG(newSVG);

    return title;
}   

//returns the description of an SVG using its fileName
char * fileNameToDesc(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * desc = malloc(sizeof(char) * (strlen(newSVG->description) + 1));
    strcpy(desc, newSVG->description);

    deleteSVG(newSVG);

    return desc;
}

//returns all the rectangles in a SVG via JSON encoded string 
char * fileNameToRectList(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * rectJSON = rectListToJSON(newSVG->rectangles);

    deleteSVG(newSVG);

    return rectJSON;
}

//returns all the circles in a SVG via JSON encoded string
char * fileNameToCircList(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * circJSON = circListToJSON(newSVG->circles);

    deleteSVG(newSVG);

    return circJSON;
}

//returns all the paths in a SVG via JSON encoded string
char * fileNameToPathList(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * pathJSON = pathListToJSON(newSVG->paths);

    deleteSVG(newSVG);

    return pathJSON;
}

//returns all the groups in a SVG via JSON encoded string
char * fileNameToGroupList(const char * fileName, const char * schemaFile)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    char * groupJSON = groupListToJSON(newSVG->groups);

    deleteSVG(newSVG);

    return groupJSON;
}

char * getCurrentShapesAttrs(const char * fileName, const char * schemaFile, int shape, int shapeNum)
{
    if(fileName == NULL || schemaFile == NULL)
    {
        return NULL;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL) // if the svg is NULL
    {
        return NULL;
    }

    if(shape == 1) //rect
    {
        ListIterator itr = createIterator(newSVG->rectangles);
        Rectangle * rect = nextElement(&itr);
        for(int i = 0; i < shapeNum; i++)
        {
            rect = nextElement(&itr);
        }
        char * attrList = attrListToJSON(rect->otherAttributes);

        free(newSVG);

        return attrList;
    }
    else if (shape == 2) //circle  
    {
        ListIterator itr = createIterator(newSVG->circles);
        Circle * circle = nextElement(&itr);
        for(int i = 0; i < shapeNum; i++)
        {
            circle = nextElement(&itr);
        }
        char * attrList = attrListToJSON(circle->otherAttributes);

        free(newSVG);

        return attrList;
    }
    else if (shape == 3) // path
    {
        ListIterator itr = createIterator(newSVG->paths);
        Path * path = nextElement(&itr);
        for(int i = 0; i < shapeNum; i++)
        {
            path = nextElement(&itr);
        }
        char * attrList = attrListToJSON(path->otherAttributes);

        free(newSVG);

        return attrList;
    }
    else if (shape == 4) // group
    {
        ListIterator itr = createIterator(newSVG->groups);
        Group * group = nextElement(&itr);
        for(int i = 0; i < shapeNum; i++)
        {
            group = nextElement(&itr);
        }
        char * attrList = attrListToJSON(group->otherAttributes);

        free(newSVG);

        return attrList;
    }
    return NULL;
}

//creates an SVG using a JSON encoded string with the title and description
int createSVGFromJSON(const char * fileName, const char * schemaFile, const char * title, const char * desc)
{
    if(fileName == NULL || schemaFile == NULL || title == NULL || desc == NULL)
    {
        return 0;
    } 

    printf("%s %s\n", title, desc);

    SVG * newSVG = malloc(sizeof(SVG));
    newSVG = initializeSVG(newSVG);

    strcpy(newSVG->title, title);
    strcpy(newSVG->description, desc);
    strcpy(newSVG->namespace, "http://www.w3.org/2000/svg");

    if(newSVG == NULL)
    {
        return 0;
    }

    bool isValid = validateSVG(newSVG, schemaFile);

    if(isValid == false)
    {
        return 0;
    }

    isValid = writeSVG(newSVG, fileName);

    deleteSVG(newSVG);
    if(isValid == true)
    {
        return 1;
    }
    return 0;
}

//updates the title and description of an SVG 
int updateTitleNDesc(const char * fileName, const char * schemaFile, const char * newTitle, const char * newDesc)
{
    if(fileName == NULL || schemaFile == NULL || newTitle == NULL || newDesc == NULL)
    {
        return 0;
    }
    
    if(strlen(newDesc) > 255 || strlen(newTitle) > 255) // makes sure the title and description are 256 chars (including NULL)
    {
        return 0;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL)
    {
        return 0;
    }

    strcpy(newSVG->title, newTitle);
    strcpy(newSVG->description, newDesc);

    bool isValid = validateSVG(newSVG, schemaFile);

    if(isValid != true)
    {
        return 0;
    }
    else
    {
        bool res = writeSVG(newSVG, fileName);

        deleteSVG(newSVG);
        if(res == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}   

//creates a new rectangle and adds it to the svg file from input from the server
int createRect(const char * fileName, const char * schemaFile, const char * x, const char * y, const char * w, const char * h, const char * fill)
{
    if(fileName == NULL || schemaFile == NULL || x == NULL || y == NULL || w == NULL || h == NULL || fill == NULL)
    {
        return 0;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL)
    {
        return 0;
    }

    float rectX = atof(x);
    float rectY = atof(y);
    float rectW = atof(w);
    float rectH = atof(h);

    Rectangle *newRect = malloc(sizeof(Rectangle)); // creates new rectangle
    strcpy(newRect->units, "\0");

    newRect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    newRect->x = rectX;
    newRect->y = rectY;
    newRect->width = rectW;
    newRect->height = rectH;

    Attribute * newAttribute = malloc(sizeof(Attribute) + (strlen(fill) * sizeof(char) + 1));
    createAttribute(newAttribute, "fill", (char*)fill);
    insertBack(newRect->otherAttributes, newAttribute);

    addComponent(newSVG, RECT, newRect);

    bool isValid = validateSVG(newSVG, schemaFile);

    if(isValid == true)
    {
        bool res = writeSVG(newSVG, fileName);

        deleteSVG(newSVG);
        if(res == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//creates a new circle and adds it to an SVG using input from the server
int createCircle(const char * fileName, const char * schemaFile, const char * cx, const char * cy, const char * cr, const char * fill)
{
    if(fileName == NULL || schemaFile == NULL || cx == NULL || cy == NULL || cr == NULL || fill == NULL)
    {
        return 0;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL)
    {
        return 0;
    }

    float circleX = atof(cx);
    float circleY = atof(cy);
    float circleR = atof(cr);

    Circle * newCirc = malloc(sizeof(Circle));
    strcpy(newCirc->units, "\0");

    newCirc->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

    newCirc->cx = circleX;
    newCirc->cy = circleY;
    newCirc->r = circleR;

    Attribute * newAttribute = malloc(sizeof(Attribute) + (strlen(fill) * sizeof(char) + 1));
    createAttribute(newAttribute, "fill", (char*)fill);
    insertBack(newCirc->otherAttributes, newAttribute);

    addComponent(newSVG, CIRC, newCirc);

    bool isValid = validateSVG(newSVG, schemaFile);

    if(isValid == true)
    {
        bool res = writeSVG(newSVG, fileName);

        deleteSVG(newSVG);
        if(res == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
// changes the scales of all the rectangles or circles in an SVG struct and updates the file
int changeShapeScale(const char * fileName, const char * schemaFile, int circCheck, const char * scale)
{
    if(fileName == NULL || schemaFile == NULL || scale == NULL)
    {
        return 0;
    }

    SVG * newSVG = createValidSVG(fileName, schemaFile);

    if(newSVG == NULL)
    {
        return 0;
    }

    float shapeScale = atof(scale);

    if(circCheck == 1) // scale circles
    {
        List * circList = getCircles(newSVG);

        if(circList == NULL)
        {
            return 0;
        }

        if(getLength(circList) == 0) // list is empty, no circles to change
        {
            return 1;
        }

        ListIterator circIter = createIterator(circList);
        for(Circle * currCirc = (Circle*)nextElement(&circIter); currCirc != NULL; currCirc = (Circle*)nextElement(&circIter))
        {
            currCirc->r = currCirc->r * shapeScale;
        }

        freeList(circList);

        bool res = writeSVG(newSVG, fileName);

        deleteSVG(newSVG);
        if(res == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else //scale rects
    {
        List * rectList = getRects(newSVG);

        if(rectList == NULL)
        {
            return 0;
        }

        if(getLength(rectList) == 0) //list is empty, no rects to change
        {
            return 1;
        }

        ListIterator rectIter = createIterator(rectList);
        for(Rectangle * currRect = (Rectangle*)nextElement(&rectIter); currRect != NULL; currRect = (Rectangle*)nextElement(&rectIter))
        {
            currRect->width = currRect->width * shapeScale;
            currRect->height = currRect->height * shapeScale;
        }

        freeList(rectList);

        bool res = writeSVG(newSVG, fileName);

        deleteSVG(newSVG);
        if(res == true)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}