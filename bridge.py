import runjs

print(runjs.run("2 + 3"))

js = None


def funca(a, b):
    """add"""
    return js.execute('return a + b', [a, b])


@js
def funcb(a, b):
    """
    return a + b;
    """
