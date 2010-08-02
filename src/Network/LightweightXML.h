/*
 *  SimpleXMLParser.h
 *  Phonosynth
 *
 *  Created by Greg Pascale on 8/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _LIGHTWEIGHTXML_H_
#define _LIGHTWEIGHTXML_H_

#include <vector>
#include <map>
#include <common.h>


class XMLException : public Exception
{
public:
	XMLException(const string& why = "Unknown XMLException",
				 int line = -1) : 
		Exception(why, line)
	{
		//cerr << "throwing XMLException at line " << line << ": " << why << endl;
	}
	
	XMLException(int line) : Exception(line)
	{
		//cerr << "throwing XMLException at line " << line << endl;
	}
	
	#define XMLTHROW(why) throw XMLException(why, __LINE__)
};

namespace XML
{
	
class Parser;
	
struct Node
{
	Node(){ }
	~Node(){ }
	
	bool		isInternal()     const { return !m_children.empty();     }
	int			getNumChildren() const { return (int) m_children.size(); }
	
	const vector<Node>& getChildren() const { return m_children; }
		
	void		setName(const string& name) { m_name = name; }
	void		setData(const string& data) { m_data = data; }
		
    const string& getName() const { return m_name; }
    const string& getData() const { return m_data; }

	Node& addChild(){
		m_children.push_back(Node()); 
		return (Node&) m_children.back(); 
	}
	
	// tag attributes
	const map<string, string>& getAttributes()								    const;
	
	bool					   getAttribute(const string& key, OUT string& ret) const;
	bool					   getAttribute(const string& key, OUT int& ret)    const;
	
	void					   setAttributes(const map<string, string>& attributes) const { m_attributes = attributes; }
	
	// data extraction
	bool		dataAsString(string& ret) const;
	bool		dataAsInt(int& ret)	      const;
	bool		dataAsFloat(float& ret)   const;
	
private:			
	
	map<string, string>			m_attributes;
	string						m_name;
	string	                    m_data;		
	vector<Node>				m_children;
	
};

struct Parsed
{
	Parsed(){ }
	~Parsed(){ }
	
	const   Node& getRoot() const { return m_root; }
	
	Node&	clearToRoot(){ return (m_root = Node()); }			
	void	printParsedXMLTree();	
	void    getAllNodesWithName(const string& name, 
								OUT vector<const Node*>& ret);
	
private:	
		
	void	printParsedXMLTreeHelper(const Node& curNode, int depth);		
	void    getAllNodesWithNameHelper(const Node& curNode,
									  const string& type, 
									  OUT vector<const Node*>& ret);
	Node	m_root;
	
};
	



class Parser
{
    
public:

	Parser(){ }
	~Parser(){ }

    // the parsing interface, call setXMLData and then parse
    void    setXMLData(const string& xmlData);
	void	setXMLData(const char* xmlData, int len);
	bool	parse(OUT Parsed& ret);	
		
	
private:		
	
    enum TokenType
    {
        TOKEN_BEGIN_TAG,
        TOKEN_END_TAG,
        TOKEN_TEXT
    };

    struct Token
    {
        TokenType			type;
        string				text;
		map<string, string> attributes;

        Token(TokenType t = TOKEN_TEXT, const string& s = "") : type(t), text(s) { }
        Token(const Token& rhs) : type(rhs.type), text(rhs.text) { }
    };

    // helpers

    // tokenizing
	bool    tokenize();
    bool    readToken(OUT Token& ret);
    
	void	parse(int firstToken, int lastToken, OUT Node& node);
	void	parseInternal(int firstToken, int lastToken, OUT Node& node);
	void	parseExternal(int firstToken, int lastToken, OUT Node& node);
	
	bool	isHeaderToken(const Token& t);
    int     findEndToken(const string& name, int firstToken, int lastToken);

	void	parseTag(const string& tag, OUT string& name);
	void	parseTag(const string& tag, OUT string& name, OUT map<string, string>& attributes);
	
    void    XMLParseError(const string& errorMsg);



    vector<Token>       m_tokens;
    string              m_xmlData;
    int                 m_curPtr;
	
}; 


inline 
void 
Parser::XMLParseError(const string& errorMsg)
{
     //cout << "XML Parse Error: " << errorMsg << endl;
     exit(1);
}
	
}; // end namespace XML

#endif _LIGHTWEIGHTXML_H_




