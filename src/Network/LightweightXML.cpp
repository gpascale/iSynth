/*
 *  SimpleXMLParser.cpp
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "LightweightXML.h"
#include <fstream>
#include <sstream>

using namespace XML;
using namespace std;


void 
Parsed::printParsedXMLTree()
{
	printParsedXMLTreeHelper(m_root, 0);
}


void 
Parsed::printParsedXMLTreeHelper(const Node& curNode, int depth)
{
    const string tabString(depth, '\t');
    //cerr << tabString << "Node: " << curNode.getName() 
        // << (curNode.isInternal() ? "(INTERNAL)" : "(EXTERNAL)") << "{" << endl;
    
    const vector<Node>& children = curNode.getChildren();

	map<string, string>::const_iterator iter;
	for(iter = curNode.getAttributes().begin(); iter != curNode.getAttributes().end(); iter++)
		//cerr << tabString << iter->first << " = " << "\"" << iter->second << "\"" << endl;
	
    if(curNode.isInternal())
        FOR(i,0,children.size())
            printParsedXMLTreeHelper(children[i], depth + 1);
    else{
		//cerr << tabString << "\t" << "Data: " << curNode.getData() << endl;
	}
		
    //cerr << tabString << "}" << endl;
}


void
Parsed::getAllNodesWithName(const string& name, OUT vector<const Node*>& ret)
{
	getAllNodesWithNameHelper(m_root, name, ret);
}


void
Parsed::getAllNodesWithNameHelper(const Node& curNode, 
								  const string& name,
								  OUT vector<const Node*>& ret)
{
	if(curNode.getName() == name)
		ret.push_back( &curNode );
	
	const vector<Node>& children = curNode.getChildren();
	FOR(i,0,children.size())
	getAllNodesWithNameHelper(children[i], name, ret);	
}


void
Parser::setXMLData(const char* xmlData, int len)
{
	setXMLData(string(xmlData, len));
}
					  
					  
void
Parser::setXMLData(const string& xmlData)
{
	m_xmlData = xmlData;
	FOR(i,0,m_xmlData.length())
		if(m_xmlData[i] == 0)
			m_xmlData[i] = ' ';
	int beg = m_xmlData.find_first_not_of("\n \r");
	int end = m_xmlData.find_last_not_of("\n \r");
	m_xmlData = m_xmlData.substr(beg, end - beg + 1);
}
					

// TODO: should parse catch the error and return false or simply propagate the error up to the caller?
bool
Parser::parse(OUT Parsed& ret)
{    
	//XML_DEBUG(cout << "data to parse = " << m_xmlData << endl);	
	
    if(!tokenize())
		return false;				
	
	Node& root = ret.clearToRoot();
	
	int startToken = 0;
	
	try{
		parse(startToken, (int) m_tokens.size() - 1, root);
	}
	catch(XMLException& e){
		return false;
	}
	
    return true;
	
}


bool
Parser::tokenize()
{    
	m_tokens.clear();
    m_curPtr = 0;
    Token t;	
    
	//Dummy begin token
	m_tokens.push_back(Token(TOKEN_BEGIN_TAG, "<DUMMY_BEGIN_TOKEN>"));
	
	try{
		while(m_curPtr < (int) m_xmlData.length() && readToken(t)){
			if(!isHeaderToken(t)){
				m_tokens.push_back(t);
				if(t.type == TOKEN_BEGIN_TAG && endsWith(t.text, "/>")){
					string name;
					map<string, string> attributes;
					parseTag(t.text, name, attributes);
					m_tokens.push_back(Token(TOKEN_END_TAG, "</" + name + ">"));
				}
			}
		}
	}
	catch(XMLException& e){
		return 0;
	}
	
	m_tokens.push_back(Token(TOKEN_END_TAG, "<DUMMY_END_TOKEN>"));
	
	return m_tokens.size() >= 2;
}


bool
Parser::readToken(OUT Token& ret)
{
	while(m_curPtr + 1 < m_xmlData.length() && 
		  (m_xmlData[m_curPtr] == ' ' 
		  || m_xmlData[m_curPtr] == '\n')) m_curPtr++;

    const int beg = m_curPtr;
          int end;

    if(m_xmlData[m_curPtr] == '<'){
        // Tag
        end = m_xmlData.find(">", m_curPtr + 1);
        if(end < 0){
            XMLTHROW("\'>\' expected!");
        }
        if(end - beg <= 1){
            XMLTHROW("Empty Tag!");
        }                
        ret.type = (m_xmlData[m_curPtr + 1] == '/') ? TOKEN_END_TAG : TOKEN_BEGIN_TAG;
		
    }
    else{
        end = m_xmlData.find("<", m_curPtr + 1);
        if(end < 0){
			return false;            
        }
        end--;  //the start carrot not actually included in the text
        ret.type = TOKEN_TEXT;
    }

    ret.text = m_xmlData.substr(beg, end - beg + 1);
    
    m_curPtr = end + 1;
	
	return true;
}


int
Parser::findEndToken(const string& name, int firstToken, int lastToken)
{
    int c = 0;
    string s;
    FOR(i, firstToken, lastToken + 1){
        if (m_tokens[i].type == TOKEN_END_TAG){
            parseTag(m_tokens[i].text, s);
            if(s == name){
                if(c == 0) return i;
                c--;
            }            
        }
        else if (m_tokens[i].type == TOKEN_BEGIN_TAG){
            parseTag(m_tokens[i].text, s);
            if(s == name)
                c++;
        }
    }
    return -1;
}


void
Parser::parse(int firstToken, int lastToken, OUT Node& node)
{    
	const Token& tok_BeginTag = m_tokens[firstToken];
    
	if(tok_BeginTag.type != TOKEN_BEGIN_TAG)
		XMLTHROW("tok_BeginTag.type != TOKEN_BEGIN_TAG");
		
    string name;
	map<string, string> attributes;
    parseTag(tok_BeginTag.text, name, attributes);
    node.setName(name);
	node.setAttributes(attributes);
	    
    int curTok = firstToken + 1;

    while(curTok < lastToken){
        if(m_tokens[curTok].type == TOKEN_TEXT){
            node.setData(m_tokens[curTok].text);
            curTok++;
        }
        else if(m_tokens[curTok].type == TOKEN_BEGIN_TAG){
            string s;
			
			parseTag(m_tokens[curTok].text, s);
		
            int endTok = findEndToken(s, curTok + 1, lastToken);
			
            if(!(endTok <= lastToken && endTok > curTok))
				XMLTHROW("endTok <= lastToken && endTok > curTok");
				
            Node& child = node.addChild();
            parse(curTok, endTok, child);
            curTok = endTok + 1;
        }
    }
}





// parse helpers

// note: this function will fail in the case where we have a tag nested inside a tag
// with the same name. Should be ok for now


bool
Parser::isHeaderToken(const Token& t)
{
	return t.type == TOKEN_BEGIN_TAG && t.text[1] == '?';
}



void 
Parser::parseTag(const string& tag, OUT string& name)
{    
	if(tag.length() <= 2)
		XMLTHROW("tag.length() <= 2");
	
	if(tag[0] != '<' || tag[tag.length() - 1] != '>')
		XMLTHROW("tag[0] != '<' || tag[tag.length() - 1] != '>'");
	
	string body;
	int beg = tag.find_first_not_of(" /<");

	if(beg == string::npos)
		XMLTHROW("beg == string::npos");
		
	body = tag.substr(beg);
	int end = body.find_first_of(" >");
	
	if(end == string::npos)
		XMLTHROW("end == string::npos");
	
	name = body.substr(0, end);
}


void 
Parser::parseTag(const string& tag, OUT string& name, OUT map<string, string>& attributes)
{    
	if(tag.length() <= 2)
		XMLTHROW("tag.length() <= 2");
	
	if(tag[0] != '<' || tag[tag.length() - 1] != '>')
		XMLTHROW("tag[0] != '<' || tag[tag.length() - 1] != '>'");     

	string body;
	int beg = tag.find_first_not_of(" /<");
	
	if(beg == string::npos)
		XMLTHROW("beg == string::npos");
	
	body = tag.substr(beg);
	int end = body.find_first_of(">");
	
	if(end == string::npos)
		XMLTHROW("end == string::npos");
	
	body = body.substr(0, end);
	
	stringstream ss(body);
	string tmp;
	int c = 0;
	while(ss >> tmp && ++c){
		if(c == 1)
			name = tmp;
		else{
			int keyBeg = 0;
			int keyEnd = tmp.find("=");
			int valBeg = keyEnd + 2;
			int valEnd = tmp.find_last_of("\"");
			const string key = tmp.substr(keyBeg, keyEnd - keyBeg);
			const string val = tmp.substr(valBeg, valEnd - valBeg);
			attributes[key] = val;
		}
	}
}





bool
Node::dataAsString(string& ret) const
{
	if(isInternal()){
		//cout << "Error: Called dataAsString on an internal node!" << endl;
		return false;
	}
	ret = m_data;
	return true;
}


const map<string, string>&
Node::getAttributes() const
{
	return m_attributes;
}


bool
Node::dataAsInt(int& ret) const
{
	if(isInternal()){
		//cout << "Error: Called dataAsString on an internal node!" << endl;
		return false;
	}
	ret = s2i(m_data);
	return true;
}


// for now, only ints. TODO: Make this generic
bool
Node::getAttribute(const string& key, OUT int& ret) const
{
	map<string, string>::const_iterator iter;
	if((iter = m_attributes.find(key)) != m_attributes.end()){
		ret = s2i(iter->second);
		return true;
	}
	return false;
}








