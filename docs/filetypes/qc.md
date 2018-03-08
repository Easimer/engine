# QC file format

Version 2

A minimal version of Valve's QC script format.

https://developer.valvesoftware.com/wiki/QC

## Line

A line consists of two parts: the command and the parameter separated by atleast one whitespace character.

`$command "parameter"`

## Command

Always starts with a `$` character and is at least 2 characters long.

## Parameter

A variable length string optionally enclosed in parantheses. Parantheses are required if the parameter
contains a whitespace character.

## Comments

Lines "commented out" are not processed by the parser. These lines always begin with two slashes: `// this is a comment`.

Valid commenting styles:
* `// comment`
* `<space><space><space>// comment`
* `$animation_idle "wolf_reference.smd" // same as reference model`

The following commenting styles are invalid:
* `/* comment */`
* `$mass 100 /* comment */`
* `# comment`

## Features missing compared to Valve's QC format
### $include
Include another QC file, C style.
### Parameter options
```
$collisionmodel "wolf.smd"
{
	$mass 100
    $concave
}
```
### $keyvalues
```
$keyvalues
{
	prop_data
	{
		base			Wooden.Small 
		dmg.bullets		0 
		explosive_damage	100
		explosive_radius	50 
	}
}
```
### Continuation lines
```
$some_vector 5.42 6.54 \\
-3.20
```
### Macros
https://developer.valvesoftware.com/wiki/$definemacro
```
$definemacro (macroname) [arg_name_1] [arg_name_2] [...] \\
```
