#include "parse.h"

using namespace wc;
using namespace wc::lex;
using namespace wc::parse;
using namespace wc::error;
using namespace std;


wc::parse::wcIdent::wcIdent()
{
}

wc::parse::wcIdent::wcIdent(std::string)
{
}

int wc::parse::getPrecedence(wcToken p_token)
{
	switch (p_token.type)
	{
	case tt_assign:
		return 1;
	case tt_logor:
		return 2;
	case tt_logand:
		return 3;
	case tt_equal:	case tt_notequal:
		return 4;
	case tt_greater:	case tt_less:	case tt_lessequal:	case tt_greaterequal:
		return 5;
	case tt_mod:	case tt_expo:
		return 6;
	case tt_plus:	case tt_minus:
		return 7;
	case tt_mult:	case tt_div:
		return 8;
	case tt_lognot:
		return 9;
	case tt_cparen:
		return 10;
	default:
		return -1;
	}
}

bool wc::parse::isRightAssociative(wcToken token)
{
	switch (token.type)
	{
	case tt_mod:
	case tt_expo:
		return true;
	}
	return false;
}

wc::parse::wcParseNode::wcParseNode()
{
	type = pn_null;
}

wc::parse::wcParseNode::wcParseNode(wcParseNodeType _type)
{
	type = _type;
}

wc::parse::wcParseNode::wcParseNode(wcParseNodeType _type, std::vector<lex::wcToken>)
{
	type = _type;
}

wc::parse::wcAST::wcAST() : parseTree(wcParseNode(pn_head))
{
	head = parseTree.head;
}

wc::parse::wcAST::wcAST(tree<wcParseNode> p_tree) : parseTree(p_tree)
{
	head = parseTree.head;
}

wcAST& wc::parse::wcAST::operator+=(wcAST otherAST)
{
	return *this = *this + otherAST;
}

wcAST wc::parse::wcAST::operator+(wcParserOutput parserOutput)
{
	wcAST tempAST(*this);

	tempAST.parseTree.append_children(head, parserOutput.ast.parseTree.begin(), parserOutput.ast.parseTree.end());

	return tempAST;
}

wcAST& wc::parse::wcAST::operator+=(wcParserOutput parserOutput)
{
	return *this = *this + parserOutput;
}

wcAST wc::parse::wcAST::operator+(wcParseNode otherNode)
{
	wcAST tempAST(*this);

	tempAST.parseTree.append_child(head, otherNode);

	return tempAST;
}

wcAST& wc::parse::wcAST::operator+=(wcParseNode otherNode)
{
	return *this = *this + otherNode;
}

wcAST wc::parse::wcAST::operator+(wcAST otherAST)
{
	wcAST tempAST(*this);

	if(otherAST.parseTree.size())
	tempAST.parseTree.append_children(head, otherAST.parseTree.begin(), otherAST.parseTree.end());

	return tempAST;
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcParserOutput pOutput)
{
	//add the tree as a child of the head node
	return addChild(wcASTIndex(*this, head), pOutput);
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcParseNode node)
{
	//add the tree as a child of the head node
	return addChild(wcASTIndex(*this, head), node);
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcASTIndex& index, wcParserOutput pOutput)
{
	//append a child to the current node, then set the index to point at that new addition
	index.set(parseTree.append_children(index.get(), pOutput.ast.parseTree.begin(), pOutput.ast.parseTree.end()));

	//return the new addition
	return index.get();
}

tree<wcParseNode>::iterator wc::parse::wcAST::addNode(wcASTIndex& index, wcParseNode node)
{
	//append a child to the current node, then set the index to point at that new addition
	index.set(parseTree.append_child(index.get(), node));

	//return the new addition
	return index.get();
}

tree<wcParseNode>::iterator wc::parse::wcAST::addChild(wcASTIndex& index, wcParseNode node)
{
	//append a child to the current node, dont set the index to it though
	parseTree.append_child(index.get(), node);

	//then return the new addition
	return parseTree.child(index.get(), parseTree.number_of_children(index.get()) - 1);
}

tree<wcParseNode>::iterator wc::parse::wcAST::addChild(wcASTIndex& index, wcParserOutput pOutput)
{
	//append a child to the current node, dont set the index to it though
	parseTree.append_children(index.get(), pOutput.ast.parseTree.begin(), pOutput.ast.parseTree.end());

	//then return the new addition
	return parseTree.child(index.get(), parseTree.number_of_children(index.get()) - 1);
}

void wc::parse::wcAST::removeNode(wcASTIndex &)
{
}

wc::parse::wcParser::wcParser()
{

}

wc::parse::wcParserIndex::wcParserIndex(lex::wcTokenStream &_input) : input(_input), ast(*new wcAST()), astIndex(ast), tokenIndex(input)
{

}

wc::parse::wcParserIndex::wcParserIndex(lex::wcTokenStream &_input, wcAST &_ast) : input(_input), ast(_ast), astIndex(ast), tokenIndex(input)
{

}

wc::parse::wcParseData::wcParseData(lex::wcTokenStream &tokens) : index(tokens), output()
{

}

wc::parse::wcParseData::wcParseData(lex::wcTokenStream &tokens, wcAST &ast) : index(tokens, ast), output()
{

}

wc::parse::wcParserOutput::wcParserOutput()
{

}

wc::parse::wcParserOutput::wcParserOutput(wcParseNode)
{
}

wc::parse::wcParserOutput::wcParserOutput(error::wcError _error)
{
	error = _error;
}

wcParserOutput wc::parse::wcParserOutput::operator+(wcParserOutput _output)
{
	wcParserOutput tempOutput(*this);

	if (_output.error)
		tempOutput.error = _output.error;
	tempOutput.ast += _output.ast;
	//tempOutput.symTab += _output.symTab;

	return tempOutput;
}

wcParserOutput& wc::parse::wcParserOutput::operator+=(wcParseNode node)
{
	return *this = *this + node;
}

wcParserOutput wc::parse::wcParserOutput::operator+(wcParseNode node)
{
	wcParserOutput tempOutput(*this);

	tempOutput.ast += node;
	tempOutput.error = error;
	//tempOutput.symTab += symTab;

	return tempOutput;
}

tree<wcParseNode>::iterator wc::parse::wcParserOutput::addNode(wcASTIndex &index, wcParserOutput output)
{
	//symTab += output.symTab;
	if (output.error)
		error = output.error;
	return ast.addNode(index, output);
}

tree<wcParseNode>::iterator wc::parse::wcParserOutput::addNode(wcASTIndex &index, wcParseNode node)
{
	return ast.addNode(index, node);
}

tree<wcParseNode>::iterator wc::parse::wcParserOutput::addChild(wcASTIndex &index, wcParserOutput output)
{
	//symTab += output.symTab;
	if (output.error)
		error = output.error;
	return ast.addChild(index, output);
}

wcParserOutput& wc::parse::wcParserOutput::operator+=(wcParserOutput _output)
{
	return *this = *this + _output;
}

wcParserOutput wc::parse::wcParser::parse(wcTokenStream &_tokens)
{
	wcParseData data(_tokens);
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;

	while (tokenIndex.isValid() && !data.output.error)
		data.output += wcSubParser().parse(data);

	return data.output;
}


wc::parse::wcSubParser::wcSubParser()
{
}

wcParserOutput wc::parse::wcSubParser::parse(wcParseData &data)
{
	wcParserOutput output;

	wcIdent ident;
	switch (data.index.input.get(data.index.tokenIndex).type)
	{
		//identifier - either dec or statement
	case tt_ident:
		wcIdentParser().parse(data, ident);
		if (data.output.symTab.exists(ident))
			switch (data.output.symTab.find(ident).type)
			{
			case st_var:	case st_func:
				goto _wcParser_parse_statements;
			case st_type:
				goto _wcParser_parse_declarations;
			default:
				return wcParserOutput(wcError(ec_par_unexpectedtoken, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));
			}
		else
			return wcParserOutput(wcError(ec_par_undeclaredident, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));

		//declarations
	_wcParser_parse_declarations:
		CASE_BASIC_TYPES_TT
			output.addNode(wcASTIndex(output.ast), wcDeclarationParser().parse(data));
		break;

		//statements
	_wcParser_parse_statements:
		CASE_ALL_LITERALS_TT
	case tt_keyword_return:
	case tt_keyword_namespace:
	case tt_keyword_if:
	case tt_keyword_while:
		output.addNode(wcASTIndex(output.ast), wcStatementParser().parse(data));
		break;

		//unexpected token
	default:
		return output;
	}

	return output;
}

wc::parse::wcStatementParser::wcStatementParser()
{
}

wcParserOutput wc::parse::wcStatementParser::parse(wcParseData &data)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;

	switch (tokens.get(tokenIndex).type)
	{
	case tt_ident:
	CASE_ALL_LITERALS_TT
		return wcExpressionParser().parse(data);

	case tt_keyword_return:
		return wcReturnParser().parse(data);

	case tt_keyword_namespace:
		return wcNamespaceParser().parse(data);

	case tt_keyword_if:
		return wcIfParser().parse(data);

	case tt_keyword_while:
		return wcWhileParser().parse(data);

	default:
		return wcParserOutput();
	}

	return wcParserOutput();
}

wcParserOutput wc::parse::wcIdentParser::parse(wcParseData &data)
{
	return parse(data, wcIdent());
}

wc::parse::wcIdentParser::wcIdentParser()
{
}

wcParserOutput wc::parse::wcIdentParser::parse(wcParseData &data, wcIdent &identOutput)
{
	//handy aliases
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;

	//while there are no ident or punctuation tokens left 
	bool punctuationLexedLast = false, isFirstToken = true;
	while (tokenIndex.isValid())
	{
		switch (tokens.get(tokenIndex).type)
		{
		case tt_dcolon:		case tt_period:
			if (!punctuationLexedLast && isFirstToken)
				return wcParserOutput(wcError(ec_par_ident_malformedident, tokens.get(tokenIndex)));	//opening token is not an identifier
			punctuationLexedLast = true;
			break;

		case tt_ident:
			punctuationLexedLast = false;
			break;

		default:
			if (isFirstToken)
				return wcParserOutput(wcError(ec_par_ident_malformedident, tokens.get(tokenIndex)));	//opening token is not an identifier
			else if (punctuationLexedLast)
				return wcParserOutput(wcError(ec_par_ident_malformedident, wcToken(tt_ident, identOutput.fullIdentifier, identOutput.line, identOutput.column)));	//malformed
			return wcParserOutput( wcParseNode(pn_ident, { wcToken(tt_ident, identOutput.fullIdentifier, identOutput.line, identOutput.column) }));
		}

		if (isFirstToken)
		{
			//save the line/column data for final token
			identOutput.line = tokens.get(tokenIndex).line;
			identOutput.column = tokens.get(tokenIndex).column;
			isFirstToken = false;
		}
		identOutput.fullIdentifier += tokens.get(tokenIndex++).data;
	}

	//if were here, input index is no longer valid, could be an error
	if (isFirstToken)
		return wcParserOutput(wcError(ec_par_ident_malformedident, identOutput.fullIdentifier));
	else if (punctuationLexedLast)
		return wcParserOutput(wcError(ec_par_ident_malformedident, identOutput.fullIdentifier));

	return wcParserOutput(wcParseNode(pn_ident, { wcToken(tt_ident, identOutput.fullIdentifier, identOutput.line, identOutput.column) }));
}

wc::parse::wcDeclarationParser::wcDeclarationParser()
{
}

wcParserOutput wc::parse::wcDeclarationParser::parse(wcParseData &data)
{
	wcParseSymbol sym;
	return parse(data, wcParseDeclaration(sym));
}

wcParserOutput wc::parse::wcDeclarationParser::parse(wcParseData &data, wcParseDeclaration& dec)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;
	wcParserOutput output;
	wcASTIndex outputIndex(output.ast);

	//parent node
	output.addNode(outputIndex, wcParseNode(pn_dec));
	if (output.error)
		return output;

	//parse the type
	wcIdent ident;
	output.addChild(outputIndex, wcTypeParser().parse(data, ident));
	if (output.error)
		return output;

	//parse the identifier
	output.addChild(outputIndex, wcIdentParser().parse(data, ident));
	if (output.error)
		return output;
	if (data.output.symTab.exists(ident))
		return wcParserOutput(wcError(ec_par_identredeclaration, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));

	//semi colon, or optional initial assignment 
	if (!tokenIndex.isValid() || (tokens.get(tokenIndex).type != tt_assign && tokens.get(tokenIndex).type != tt_scolon))
		return output;
	else if (tokens.get(tokenIndex).type == tt_scolon)
		return (output += wcSemiColonParser().parse(data));
	tokenIndex++;

	//expression node
	output.addNode(outputIndex, wcExpressionParser().parse(data));
	if (output.error)
		return output;

	return output;
}

wc::parse::wcTypeParser::wcTypeParser()
{
}

wcParserOutput wc::parse::wcTypeParser::parse(wcParseData &data, wcIdent &ident)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;
	wcParserOutput output;
	wcASTIndex outputIndex = wcASTIndex(output.ast);

	switch (tokens.get(tokenIndex).type)
	{
	case tt_ident:
		output.ast.addChild(outputIndex, wcIdentParser().parse(data, ident));
		if (!data.output.symTab.exists(ident))
			return wcParserOutput(wcError(ec_par_type_undeclaredtype, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));
		if (data.output.symTab.find(ident).type != st_type)
			return wcParserOutput(wcError(ec_par_unexpectedtoken, wcToken(tt_ident, ident.fullIdentifier, ident.line, ident.column)));
		return output;

		//declarations
		CASE_BASIC_TYPES_TT
			output.ast.addChild(outputIndex, wcParseNode(pn_type, { tokens.get(tokenIndex) }));
		tokenIndex++;
		return output;
	}

	return wcParserOutput(wcError(ec_par_type_eos));
}

wc::parse::wcSemiColonParser::wcSemiColonParser()
{
}

wcParserOutput wc::parse::wcSemiColonParser::parse(wcParseData &data)
{
	if (data.index.input.get(data.index.tokenIndex).type != tt_scolon)
		return wcParserOutput(wcError(ec_par_unexpectedtoken, data.index.input.get(data.index.tokenIndex)));

	data.index.tokenIndex++;

	return wcParserOutput() += wcParseNode(pn_scolon);
}

wcParserOutput wc::parse::wcTypeParser::parse(wcParseData &data)
{
	wcIdent ident;
	return parse(data, ident);
}

wc::parse::wcReturnParser::wcReturnParser()
{
}

wcParserOutput wc::parse::wcReturnParser::parse(wcParseData &data)
{
	if (data.index.input.get(data.index.tokenIndex).type != tt_keyword_return)
		return wcParserOutput(wcError(ec_par_unexpectedtoken, data.index.input.get(data.index.tokenIndex)));

	data.index.tokenIndex++;

	return wcParserOutput() += wcParseNode(pn_return);
}

wc::parse::wcCodeBlockParser::wcCodeBlockParser()
{
}

wcParserOutput wc::parse::wcCodeBlockParser::parse(wcParseData &data)
{
	//create our stream indexes, and space for output data, and a handy alias (tokens)
	wcTokenStream& tokens = data.index.input;
	wcTokenStreamIndex& tokenIndex = data.index.tokenIndex;
	wcParserOutput output;
	wcASTIndex outputIndex = wcASTIndex(output.ast);

	//consume opening bracket
	if (tokens.get(tokenIndex).type != tt_obracket)
		return wcParserOutput(wcError(ec_par_unexpectedtoken, tokens.get(tokenIndex)));
	tokenIndex++;

	//parse statements until we find closing bracket
	while (tokenIndex.isValid() && !output.error)
		if (tokens.get(tokenIndex).type == tt_cbracket)
		{
			tokenIndex++;
			return output;
		}
		else
			output.addChild(outputIndex, wcSubParser().parse(data));

	//error no closing bracket
	return wcParserOutput(wcError(ec_par_eos));
}


wc::parse::wcParseDeclaration::wcParseDeclaration(wcParseSymbol &_symbol) : type(_symbol)
{

}

wc::parse::wcWhileParser::wcWhileParser()
{
}

wcParserOutput wc::parse::wcWhileParser::parse(wcParseData &)
{
	return wcParserOutput();
}

wc::parse::wcIfParser::wcIfParser()
{
}

wcParserOutput wc::parse::wcIfParser::parse(wcParseData &)
{
	return wcParserOutput();
}

wc::parse::wcNamespaceParser::wcNamespaceParser()
{
}

wcParserOutput wc::parse::wcNamespaceParser::parse(wcParseData &)
{
	return wcParserOutput();
}

wc::parse::wcExpressionParser::wcExpressionParser()
{
}

wcParserOutput wc::parse::wcExpressionParser::parse(wcParseData &)
{
	return wcParserOutput();
}

wc::parse::wcASTIndex::wcASTIndex(wcAST &_ast) : ast(_ast)
{
	index = ast.parseTree.begin();
}

wc::parse::wcASTIndex::wcASTIndex(wcAST &_ast, tree<wcParseNode>::iterator _index) : ast(_ast)
{
	index = _index;
}

wcASTIndex wc::parse::wcASTIndex::operator-(int subtraction)
{
	tree<wcParseNode>::iterator newIndex = index;
	for (int t = 0; t < subtraction; ++t)
		newIndex--;
	return wcASTIndex(ast, newIndex);
}

wcASTIndex wc::parse::wcASTIndex::operator+(int addition)
{
	tree<wcParseNode>::iterator newIndex = index;
	for (int t = 0; t < addition; ++t)
		newIndex++;
	return wcASTIndex(ast, newIndex);
}

wcASTIndex& wc::parse::wcASTIndex::operator=(tree<wcParseNode>::iterator _index)
{
	index = _index;
	return *this;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::operator[](tree<wcParseNode>::iterator _index)
{
	return tree<wcParseNode>::iterator();
}

wcASTIndex& wc::parse::wcASTIndex::operator++()
{
	index++;
	return *this;
}

wcASTIndex& wc::parse::wcASTIndex::operator--()
{
	index--;
	return *this;
}

wcASTIndex& wc::parse::wcASTIndex::operator=(wcASTIndex otherIndex)
{
	ast = otherIndex.ast;
	index = otherIndex.index;
	return *this;
}


wcASTIndex wc::parse::wcASTIndex::operator++(int)
{
	return *this = *this + 1;
}

wcASTIndex wc::parse::wcASTIndex::operator--(int)
{
	return *this = *this - 1;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::get()
{
	return index;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::next()
{
	return index++;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::prev()
{
	return index--;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::set(tree<wcParseNode>::iterator newIndex)
{
	return index = newIndex;
}

tree<wcParseNode>::iterator wc::parse::wcASTIndex::up()
{
	if (index->parent != nullptr)
		return (index = index->parent);
	return index;
}

int wc::parse::wcASTIndex::depth()
{
	return ast.parseTree.depth(index);
}

int wc::parse::wcASTIndex::depth(tree<wcParseNode>::iterator)
{
	return 0;
}

wcParserSymbolTable & wc::parse::wcParserSymbolTable::operator+=(wcParserSymbolTable otherTable)
{
	return *this = *this + otherTable;
}

wcParserSymbolTable wc::parse::wcParserSymbolTable::operator+(wcParserSymbolTable otherTable)
{
	wcParserSymbolTable tempTable(*this);

	for (auto iter = otherTable.lookup.begin(); iter != otherTable.lookup.end(); ++iter)
		if (tempTable.exists(iter->first))
			tempTable.reg(iter->second);

	return tempTable;
}

wcParserSymbolTable & wc::parse::wcParserSymbolTable::operator+=(wcParseSymbol symbol)
{
	return *this = *this + symbol;
}

wcParserSymbolTable wc::parse::wcParserSymbolTable::operator+(wcParseSymbol symbol)
{
	wcParserSymbolTable tempTable(*this);

	if (!tempTable.exists(symbol.ident))
		tempTable.reg(symbol);

	return tempTable;
}

bool wc::parse::wcParserSymbolTable::exists(wcIdent ident)
{
	if (lookup.find(ident.fullIdentifier) == lookup.end())
		return false;
	else
		return true;
}

wcParseSymbol& wc::parse::wcParserSymbolTable::find(wcIdent ident)
{
	return lookup.find(ident.fullIdentifier)->second;
}

wcParseSymbol wc::parse::wcParserSymbolTable::reg(wcParseSymbol symbol)
{
	lookup.insert(make_pair(symbol.ident.fullIdentifier, symbol));
	return find(symbol.ident.fullIdentifier);
}
