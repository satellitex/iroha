
from enum import Enum
import random
import hashlib
import base64
import itertools
import sys

code_prefix = """
/*
Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <gtest/gtest.h>
#include <iostream>
#include <string> 
#include <climits>
"""

class Type(Enum):
    INT = 1
    STR = 2
    BOOL = 3

class Case(Enum):
    SMALL = 1
    BIG = 2
    RANDOM = 3

cases = [
    Case.SMALL,
    Case.BIG,
    Case.RANDOM
]
caseStr = {
    Case.SMALL: "Small",
    Case.BIG:   "Big",
    Case.RANDOM:"Random" 
}
typeStr = {
    Type.INT:   "int",
    Type.STR:   "std::string",
    Type.BOOL:  "bool"
}

def makeSample( t: Type, c: Case):
    if t == Type.INT:
        if c == Case.SMALL:
            return '0'
        elif c == Case.BIG: 
            return 'INT_MAX'
        else:
            return str(random.randint(1,pow(2,31)-1))
    elif t == Type.STR:
        if c == Case.SMALL:
            return '""'
        elif c == Case.BIG: 
            return '"'+ "A" * 500 +'"'
        else:
            return '"'+ base64.b32encode(hashlib.md5(str(random.random()).encode('utf-8')).hexdigest().encode('utf-8')).decode('utf-8') +'"'         
    elif t == Type.BOOL:
        if c == Case.SMALL:
            return "false"
        elif c == Case.BIG: 
            return "true"
        else:
            return "true" if random.randint(0,1) else "false"  


def makeTestCase(code, baseName, detailName, methods, cases):    
    code += "TEST( " + baseName + ", " + detailName + ") {\n\n"
    code += "    auto "+baseName.lower() + " = "+ baseName + "Builer()\n"

    assert len(methods) == len(cases)
    expects = []

    for i in range(len(methods)):
        expects.append(makeSample(methods[i]["type"], cases[i]))
        code += "      .set"+ methods[i]["name"] +"("+ expects[-1] +")\n"
    code += "      .build();\n\n"


    for i in range(len(methods)):
        if methods[i]["type"] == Type.STR:
            code += "    ASSERT_STREQ(" + baseName.lower() +"->"+ methods[i]["name"].lower() +"->c_str(), "+ expects[i] +")\n"
        else:
            code += "    ASSERT_STREQ(" + baseName.lower() +"->"+ methods[i]["name"].lower() +", "+ expects[i] +")\n"
    code += "}\n\n"
    return code

def makeHeader( code, targetName, members):
    """
    This function create test code for targetNames builder.
    =====
    Usage:
     makeTase4Builder(code, BuildTergetName = "Sample", [
        {
            "name": Name,
            "type": Type.STR, Type.INT, or Type.BOOL
        }
    ])
    """
    members_len = len(members)
    code += "class "+targetName+" {\n\n"

    code += "public:\n"

    for m in members:
        code += "    "+ targetName +"* set"+ m["name"] +"(" + typeStr[m["type"]] +");\n"

    code += "\n"
    code += "    "+ targetName +" build();\n"
    code += "\n}\n"
    return code

def makeTase4Builder( code: str, targetName: str, methods: list):
    """
    This function create test code for targetNames builder.
    =====
    Usage:
     makeTase4Builder(code, BuildTergetName = "Sample", [
        {
            "name": Name,
            "type": Type.STR, Type.INT, or Type.BOOL
        }
    ])
    """
    method_len = len(methods)
    for methodList in itertools.permutations(methods, method_len):
        for caseList in itertools.combinations_with_replacement( cases, method_len):
            detail = ""
            for i in range(method_len):
                detail += methodList[i]["name"] + caseStr[caseList[i]] +"_"
            code = makeTestCase( code, targetName, detail[:-1],methodList,caseList)

    return code

if __name__ == "__main__":
    code = code_prefix
    
    code = makeTase4Builder(code,"Asset", [
    #ode = makeHeader(code,"Asset", [
        {
            "name":"DomainName",
            "type":Type.STR
        },
        {
            "name":"Name",
            "type":Type.STR
        },
        {
            "name":"Options",
            "type":Type.STR
        }
    ])
    print(code)
