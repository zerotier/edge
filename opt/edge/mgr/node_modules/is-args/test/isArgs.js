var isArgs = require('..')
  , should = require('should')
;

describe('isArgs', function () {
    
    var args = (function () { return arguments; })(1, 2, 3);
    
    it('should return true for arguments', function () {
        isArgs(args).should.be.true;
    });
    
    it('should return false for everything else', function () {
        [
            undefined,
            null,
            'abc',
            123,
            true,
            false,
            [1, 2, 3],
            { a: 1, b: 2, c: 3 }
        ].forEach(function (val) {
            isArgs(val).should.be.false;
        });
    });
    
    it('similar objects unfortunatly pass as arguments', function () {
        var arr = [1, 2, 3];
        arr.callee = function () {};
        isArgs(arr).should.be.true;
    });
    
});