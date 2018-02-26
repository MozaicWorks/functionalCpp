#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <list>
#include <algorithm>
#include <functional>
#include <numeric>

using namespace std;
using namespace std::placeholders;

using AList=list<int>;

template<typename Collection, typename Lambda>
const Collection simplifiedTransform(const Collection& sourceCollection, const Lambda&& lambda){
    Collection result;
    result.resize(sourceCollection.size());
    transform(sourceCollection.begin(), sourceCollection.end(), result.begin(), lambda);
    return result;
}

template<typename SourceCollection, typename DestinationCollection, typename Lambda>
const DestinationCollection simplifiedTransform(const SourceCollection& sourceCollection, const Lambda&& lambda){
    DestinationCollection result;
    result.resize(sourceCollection.size());
    transform(sourceCollection.begin(), sourceCollection.end(), result.begin(), lambda);
    return result;
}

auto incrementAllElements(const list<int> aList){
    return simplifiedTransform<AList>(aList,
        [](const int item){ 
            return item + 1;
            }
    );
}

TEST_CASE("increment all elements using transform"){
    CHECK(incrementAllElements(AList({0, 1, 2})) == AList({1, 2, 3}));
} auto increment = [](auto value){return value + 1;};

TEST_CASE("increment a value using lambda"){
    CHECK(increment(2) == 3);
}

int doAppend(int valueFromContext){
    auto appendValueFromContext = [valueFromContext](int value){
        return value + valueFromContext;
    };
    return appendValueFromContext(10);
}

TEST_CASE("append value from context"){
    CHECK(doAppend(10) == 20);
}

int doAppendWithAutoCapture(int valueFromContext){
    auto appendValueFromContext = [=](int value){
        return value + valueFromContext;
    };
    return appendValueFromContext(10);
}

TEST_CASE("append value from context with auto capture"){
    CHECK(doAppendWithAutoCapture(10) == 20);
}

auto add = [](const list<int>& initial, const int value){
    list<int> result(initial);
    result.push_back(value);
    return result;
};

TEST_CASE("add elements to list by chaining"){
    CHECK(add(add(add(list<int>(), 1), 2), 3) == list<int>({1, 2, 3}));
}

auto addToEmptyList = bind(add, list<int>(), _1);

TEST_CASE("add elements with curry"){
    CHECK(addToEmptyList(1) == list<int>({1}));
    CHECK(addToEmptyList(2) == list<int>({2}));

    CHECK(add(addToEmptyList(1), 2) == list<int>({1, 2}));
}


auto threeElementsList = [](const int first, const int second, const int third){
    return add(add(addToEmptyList(first), second), third);
};

TEST_CASE("compose functions"){
    CHECK(threeElementsList(0, 1, 2) == AList({0, 1, 2}));
}

auto addTwoElementsToEmptyList = [](const int first, const int second){
    return add(addToEmptyList(first), second);
};

auto listWith0And1 = bind(addTwoElementsToEmptyList, 0, 1);

TEST_CASE("compose and curry"){
    CHECK(listWith0And1() == AList({0, 1}));
}

auto incrementFunctionResult = [](const auto aFunction){
    return aFunction() + 1;
};

auto function1 = [](){
    return 1;
};

TEST_CASE("call function with function parameter"){
    CHECK(incrementFunctionResult(function1) == 2);
}

TEST_CASE("find_if (more like find first)"){
    AList aList = threeElementsList(0, 1, 2);
    auto found = find_if(aList.begin(), aList.end(), [](auto element){return element % 2 != 0;} );
    CHECK(*found == 1);
}

TEST_CASE("reduce"){
    AList aList = threeElementsList(0, 1, 2);
    list<string> convertedToString = simplifiedTransform<AList, list<string>>(aList, [](int element) {return to_string(element);});

    // Doesn't work with g++ yet
    /*string value = reduce(
            convertedToString.begin(), 
            convertedToString.end(),
            string(),
            [](string first, string second){return first + second;}
            );*/
    
    string value = accumulate(
            convertedToString.begin(), 
            convertedToString.end(),
            string(),
            [](string first, string second){return first + second;}
            );


    CHECK(value == "012");
}

TEST_CASE("count_if"){
    AList aList = threeElementsList(0, 1, 2);

    CHECK(count_if(aList.begin(), aList.end(), [](const int element){return element % 2 != 0;}) == 1);
}

TEST_CASE("all_of"){
    AList aList = threeElementsList(0, 1, 2);

    CHECK(all_of(aList.begin(), aList.end(), [](const int element){return element < 3;}));
    CHECK(!all_of(aList.begin(), aList.end(), [](const int element){return element <= 1;}));
}

TEST_CASE("any_of"){
    AList aList = threeElementsList(0, 1, 2);

    CHECK(any_of(aList.begin(), aList.end(), [](const int element){return element == 0;}));
    CHECK(!any_of(aList.begin(), aList.end(), [](const int element){return element > 2;}));
}

TEST_CASE("none_of"){
    AList aList = threeElementsList(0, 1, 2);

    CHECK(none_of(aList.begin(), aList.end(), [](const int element){return element == 5;}));
    CHECK(!none_of(aList.begin(), aList.end(), [](const int element){return element == 2;}));
}
