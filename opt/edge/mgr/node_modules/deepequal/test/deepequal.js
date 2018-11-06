var deepEqual = require('..')
  , should = require('should')
;

describe('deepequal', function () {
    
    it('should pass all `deep-equal` tests', function () {
        
        deepEqual(
            { a : [ 2, 3 ], b : [ 4 ] },
            { a : [ 2, 3 ], b : [ 4 ] }
        ).should.be.true;
        
        deepEqual(
            { x : 5, y : [6] },
            { x : 5, y : 6 }
        ).should.be.false;
        
        deepEqual(
            [ null, null, null ],
            [ null, null, null ]
        ).should.be.true;
        
        deepEqual(
            [ { a: 3 }, { b: 4 } ],
            [ { a: '3' }, { b: '4' } ],
            true
        ).should.be.false;
        
        deepEqual(3, 3).should.be.true;
        deepEqual('beep', 'beep').should.be.true;
        deepEqual('3', 3).should.be.true;
        deepEqual('3', 3, true).should.be.false;
        deepEqual('3', [3]).should.be.false;
        
        deepEqual(
            (function(){return arguments})(1,2,3),
            (function(){return arguments})(1,2,3)
        ).should.be.true;
        deepEqual(
            (function(){return arguments})(1,2,3),
            [1,2,3]
        ).should.be.false;
        deepEqual(
            [1,2,3],
            (function(){return arguments})(1,2,3)
        ).should.be.false;
        
        var d0 = new Date(1387585278000);
        var d1 = new Date('Fri Dec 20 2013 16:21:18 GMT-0800 (PST)');
        
        deepEqual(d0, d1).should.be.true;
        
    });
    
    it('should return true for NaN', function () {
        deepEqual(NaN, NaN).should.be.true;
        deepEqual({ a: NaN }, { a: NaN }).should.be.true;
        
        deepEqual(123, NaN).should.be.false;
        deepEqual(NaN, 'abc').should.be.false;
        
        deepEqual('abc', NaN).should.be.false;
    });
    
    it('should compare buffers', function () {
        var b1 = new Buffer('foobar')
          , b2 = new Buffer('foobar')
          , b3 = new Buffer('foobaz')
        ;
        
        deepEqual(b1, b2).should.be.true;
        deepEqual(b1, b3).should.be.false;
        
        deepEqual(b1, 'foobar').should.be.false;
        deepEqual('foobar', b1).should.be.false;
    });
    
    it('should inspect all values of an array', function () {
        var a = [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ]
          , b = a.slice()
          , c = a .slice()
        ;
        
        c[7] = 3;
        
        deepEqual(a, b).should.be.true;
        deepEqual(a, c).should.be.false;
        
        deepEqual('abc', a).should.be.false;
    });
    
    it('should compare regular expressions', function () {
        
        var a = /abc/gi
          , b = new RegExp('abc', 'gi')
          , c = /abc/gim
          , d = /def/gi
          , e = /abc/gi
        ;
        
        e.test('foo abc bar');
        
        deepEqual(a, b).should.be.true;
        deepEqual(a, c).should.be.false;
        deepEqual(a, d).should.be.false;
        deepEqual(a, e).should.be.false;
        
    });
    
    it('should compare null and undefined', function () {
        deepEqual(null, null).should.be.true;
        deepEqual(undefined, undefined).should.be.true;
        
        deepEqual(null, undefined).should.be.false;
        deepEqual(null, false).should.be.false;
        
        deepEqual({ a: 1 }, null).should.be.false;
        deepEqual({ a: 1 }, undefined).should.be.false;
    });
    
});