var fastApply = require('fast-apply')
  , isArgs = require('is-args')
  , isBuffer = typeof Buffer === 'function' ? Buffer.isBuffer : null
;

module.exports = deepEqual;

function deepEqual(actual, expected, strict) {
    if (actual === expected) return true;
    
    // isNaN test
    if (actual !== actual && expected !== expected) return true;
    
    var actualType = typeof actual
      , expectedType = typeof expected
      , i
    ;
    
    if (actualType !== 'object' && expectedType !== 'object') return strict ? actual === expected : actual == expected;
    
    // null is an object, but cannot have properties; stop here
    if (actual === null || expected === null) return false;
    
    if (actualType !== expectedType) return false;
    
    if (actual.prototype !== expected.prototype) return false;
    
    if (actual instanceof Date) return actual.getTime() === expected.getTime();
    
    if (actual instanceof RegExp) {
        return actual.source === expected.source
            && actual.lastIndex === expected.lastIndex
            && actual.global === expected.global
            && actual.multiline === expected.multiline
            && actual.ignoreCase === expected.ignoreCase
        ;
    }
    
    if (isBuffer && isBuffer(actual)) {
        if (actual.length !== expected.length) return false;
        
        i = actual.length;
        
        while (--i >= 0) {
            if (actual[i] !== expected[i]) return false;
        }
        
        return true;
    }
    
    var actualArg = isArgs(actual)
      , expectedArg = isArgs(expected)
    ;
    
    if (actualArg || expectedArg) {
        if (!actualArg || !expectedArg) return false;
        actual = fastApply(Array, null, actual);
        expected = fastApply(Array, null, expected);
    }
    
    var actualKeys = Object.keys(actual)
      , expectedKeys = Object.keys(expected)
      , key
    ;
    
    if (actualKeys.length !== expectedKeys.length) return false;
    
    actualKeys.sort();
    expectedKeys.sort();
    
    i = actualKeys.length;
    
    while (--i >= 0) {
        if (actualKeys[i] !== expectedKeys[i]) return false;
    }
    
    i = actualKeys.length;
    
    while (--i >= 0) {
        key = actualKeys[i];
        if (!deepEqual(actual[key], expected[key], strict)) return false;
    }
    
    return true;
    
}