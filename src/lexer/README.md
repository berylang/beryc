## Bery Lexical Analysis

This readme is intended to give you sufficient knowledge about Bery's Lexical analysis a.k.a Lexer. Bery's lexer follows simple modular structure having different method/function for different scanning tasks.


#### Maximal Munch rule
Lexer follows maximal munch trick to emit the correct tokens in a single pass. 

e.g. if lexer finds '+' then it look ahead for next symbol, if it is '+' it emits `TOKEN_INC`, if it is '=' it emits `TOKEN_EQUAL`, if it is neither then it emits `TOKEN_PLUS`.

---


#### Whitespaces & Comments rule
Lexer intentionally drops every whitespaces (' ', \t, \n, \r), and every comment block (--- Single line comment) or (--! multi line comment block !--) and only emits the token the langauge supports.



#### Tokenization rule

|category | Rule |
|---|---|
|whitepsaces | ignored, except for line couting|
|Comments| ignored|
|Keywords|Recognised after identifier scanning|
|Identifiers|Leter or _ followed by leeters, digit or _|
|Operators| maximal munch|


#### Keywords 
| | | | | | |
|--|--|--|--|--|--|
|int|float|bigint|double|bool|char|
|string|run|true|false|null|if|
|else|switch|case|default|break|continue|
|pass|while|do|for|in|func|
|return|enum|import|extern|class|attributes|
|methods|new|public|private|protected||


#### COmplexity analysis

- Scanning : Single pass
- Token Recognization : Maximal Munch
- Time complexity : O(n)
- Keyword lookup : O(1)
- Memory Complexity : O(tokens)


#### Error Handling

`yet to be added`