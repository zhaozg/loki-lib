///////////////////////////////////////////////////////////////////////////////
// Unit Test for Loki
//
// Copyright Terje Sletteb? and Pavel Vozenilek 2002.
// Copyright Peter K?mmel, 2006

// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef FUNCTORTEST_H
#define FUNCTORTEST_H

// $Id$


#include <loki/Functor.h>

///////////////////////////////////////////////////////////////////////////////
// FunctorTest
///////////////////////////////////////////////////////////////////////////////

void free_function(bool &result)
{
    result=true;
}

class FunctorTest : public Test
{
public:
    FunctorTest() : Test("Functor.h")
    {}

    virtual void execute(TestResult &result)
    {
        printName(result);

        using namespace Loki;

        bool r;

        TestFunctor testFunctor;
        TestClass testClass;


        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> function(testFunction);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> function2(testFunction);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> functor(testFunctor);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> functor2(testFunctor);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> classFunctor(&testClass,&TestClass::member);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> classFunctor2(&testClass,&TestClass::member);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> functorCopy(function);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> functorCopy2(function);

        //TODO:
        // BindFirst and Chainer

        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> member_func(&testClass,&TestClass::member);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> free_func(&free_function);
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> NULL_func;
        Functor<void, LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, bool &> NULL_func0;

        testResult=false;
        function(testResult);
        bool functionResult=testResult;

        testResult=false;
        functor(testResult);
        bool functorResult=testResult;

        testResult=false;
        classFunctor(testResult);
        bool classFunctorResult=testResult;

        testResult=false;
        functorCopy(testResult);
        bool functorCopyResult=testResult;

#ifdef LOKI_FUNCTORS_ARE_COMPARABLE

        bool functionCompare = function==function2;
        bool functorCompare = functor!=functor2;  // is this a bug?
        bool classFunctorCompare = classFunctor==classFunctor2;
        bool functorCopyCompare = functorCopy==functorCopy2;

        bool free_mem = free_func!=member_func;
        bool mem_free = member_func!=free_func;

        bool null0 = NULL_func == NULL_func0;
        bool null1 = NULL_func != free_func;
        bool null2 = NULL_func != member_func;
        bool null3 = free_func != NULL_func;
        bool null4 = member_func != NULL_func;


        bool compare =  functionCompare &&
                        functorCompare &&
                        classFunctorCompare &&
                        functorCopyCompare &&
                        mem_free &&
                        free_mem &&
                        null0 &&
                        null1 &&
                        null2 &&
                        null3 &&
                        null4
                        ;
#else

        bool compare=true;
#endif //LOKI_FUNCTORS_ARE_COMPARABLE


       //TODO!
        r=functionResult && functorResult && classFunctorResult && functorCopyResult && compare;

        testAssert("Functor",r,result);

        std::cout << '\n';
    }

private:
    static bool testResult;

    static void testFunction(bool &result)
    {
        result=true;
    }

    class TestFunctor
    {
    public:
        void operator()(bool &result)
        {
            result=true;
        }
        bool operator==(const TestFunctor& rhs) const
        {
            const TestFunctor* p = &rhs;
            return this==p;
        }
    };

    class TestClass
    {
    public:
        void member(bool &result)
        {
            result=true;
        }
    };
}
functorTest;

bool FunctorTest::testResult;

#ifndef SMALLOBJ_CPP
# define SMALLOBJ_CPP
# include "../../src/SmallObj.cpp"
#endif
#endif
