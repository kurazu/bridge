import unittest
from bridge import js_func


@js_func
def add(a, b):
    """
    var sum;
    sum = a + b;
    return sum;
    """


@js_func
def get_prop(mapping, property_name):
    """
    return mapping[property_name];
    """


@js_func
def mul(a, b=2):
    """
    return a * b;
    """


@js_func
def runtime(a, b):
    """
    /* Notice that c was not defined. */
    return b + c;
    """


@js_func
def set(val):
    """
    xxx = val; // Set global variable.
    """


@js_func
def get():
    """
    return xxx; // Read global variable.
    """


@js_func
def math(str):
    """
    var sqrt = Math.sqrt;
    return (function(arg) {
        return sqrt(arg);
    })(str);
    """


class TestJS(unittest.TestCase):

    def test_add(self):
        self.assertEqual(add(2, 3), 5)
        self.assertEqual(add(2.5, 3.75), 6.25)
        with self.assertRaises(TypeError):
            add(2)
        with self.assertRaises(TypeError):
            add()
        with self.assertRaises(TypeError):
            add(2, 3, 4)
        self.assertEqual(add('hello', 'world'), 'helloworld')
        self.assertEqual(add({}, {}), '[object Object][object Object]')

    def test_get_prop(self):
        d = {'a': 1, 'b': 2}
        a = [7, 8, 9]
        self.assertEqual(get_prop(d, 'a'), 1)
        self.assertEqual(get_prop(d, 'b'), 2)
        self.assertEqual(get_prop(d, 'c'), None)
        self.assertEqual(get_prop(a, 'length'), 3)
        self.assertEqual(get_prop(a, '0'), 7)
        self.assertEqual(get_prop(a, 1), 8)
        self.assertEqual(get_prop(a, 2), 9)
        self.assertEqual(get_prop(a, 3), None)
        self.assertEqual(get_prop(a, {}), None)

    def test_default_arg(self):
        self.assertEqual(mul(2, 3), 6)
        self.assertEqual(mul(4), 8)
        with self.assertRaises(TypeError):
            mul()
        with self.assertRaises(TypeError):
            mul(2, 3, 4)
        # JSON serialization of NaN is null.
        self.assertEqual(mul(8, {}), None)

    def assertStartsWith(self, phrase, pattern):
        msg = '{!r} does not start with {!r}'.format(phrase, pattern)
        self.assertTrue(phrase.startswith(pattern), msg)

    def assertEndsWith(self, phrase, pattern):
        msg = '{!r} does not end with {!r}'.format(phrase, pattern)
        self.assertTrue(phrase.endswith(pattern), msg)

    def test_syntax_errors(self):
        try:
            @js_func
            def syntax(a, b):
                """
                /* Notice the @ */
                return @a + b;
                """
        except RuntimeError as ex:
            msg, = ex.args
            self.assertStartsWith(msg, 'SyntaxError: illegal character at')
            self.assertEndsWith(msg, 'bridge/tests.py:111')
        else:
            self.fail('Exception not raised')

    def test_runtime_errros(self):
        try:
            runtime(5, 6)
        except RuntimeError as ex:
            msg, = ex.args
            self.assertStartsWith(msg, 'ReferenceError: c is not defined at')
            self.assertEndsWith(msg, 'bridge/tests.py:32')
        else:
            self.fail('Exception not raised')

    def test_set_get(self):
        set(17)
        self.assertEqual(get(), 17)

    def test_math(self):
        self.assertEqual(math(4), 2.0)
        self.assertEqual(math(9), 3.0)
        self.assertEqual(math(12.25), 3.5)


if __name__ == '__main__':
    unittest.main()
