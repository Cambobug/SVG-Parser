#include "SVGParser.h"
#include <ctype.h>
#include <math.h>

//functions that deal with SVGs
SVG * createSVGTree(xmlDoc *);
void createAttribute(Attribute*, char*, char*); // creates a populates an attribute to add to a list
SVG * initializeSVG(SVG*); // initializes all of the SVG structs lists
SVG * traverseTree(xmlNode*, SVG*, Group*); // traverses through svg file
List * searchGroups(List*, List*, int , int , int , int); // searches through a list of groups for the specified element
void traverseGroupsForLen(List* , int* , int); //traverses through each group and finds the number of elements in each
void traverseGroupsForAttrs(List *, int * ); // totals up all the attributes in the list of groups passed to it
bool checkForNegatives(const SVG * img);

//functions that deal with xml and schema files
xmlDoc * createXmlDoc(SVG *, xmlDoc *, xmlNode *); // 
bool validateXmlTree(xmlDoc *, const char *);
xmlDoc * svgToXmlDoc(const SVG *, xmlDoc *, xmlNode *, Group *);

//helper functions
float checkForUnits(char*, char*); // checks for and removes units from a measurement
char * catStringNFree(char * , char * , int); // concatenates 2 strings and then frees 1 string
void dummyDelete();