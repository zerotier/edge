# deepequal

Deep comparison of two variables

Unlike [deep-equal](https://github.com/substack/node-deep-equal), this will also compare Buffers,
Regular Expressions, and NaN (which I believe should equal NaN).

```javascript

var deepEqual = require('deepequal');

deepEqual([1, 2, 3], new Array(1, 2, 3));
// true;

deepEqual(NaN, NaN);
// true;

deepEqual({ a: [1, 2, 3] }, { a: [1, 3, 2] });
// false;

```

## Installation

    $ npm install deepequal

## Dependencies

  - [fast-apply](https://npmjs.org/package/fast-apply)
  - [is-args](https://npmjs.org/package/is-args)

## API

### deepEqual(a, b, strict)

  - strict - Use strict comparison (===) between primitives (Boolean, String, Number) (default: false)