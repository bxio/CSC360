/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


final public class Math
{
    final public static float E  = (float)2.7182818284590452354;
    final public static float PI = (float)3.14159265358979323846;
//  public static Random randomGenerator = new Random();

//  native public static double IEEEremainder(double f1, double f2);

    public static float abs(float a)
    {
        if (a < 0.0f)
        {
            return -a;
        }
        return a;
    }

    public static int abs(int a)
    {
        if (a < 0)
        {
            return -a;
        }
        return a;
    }


    public static float max(float a, float b)
    {
        if (b > a)
        {
            return b;
        }
        return a;
    }

    public static int max(int a, int b)
    {
        if (b > a)
        {
            return b;
        }
        return (a);
    }

    public static float min(float a, float b)
    {
        if (a < b)
        {
            return a;
        }
        return b;
    }

    public static int min(int a, int b)
    {
        if (a < b)
        {
            return a;
        }
        return b;
    }

/*
    To add support for the following native method add their definitions
    into ClassLinker.control file (located in lib\ClassLinker.jar) as weel as
    into j_lang.c. Make sure that the order of methods is the same is in
    ClassLinker.control.

    native public static float sin(float a);
    native public static float asin(float a);
    native public static float cos(float a);
    native public static float acos(float a);
    native public static float tan(float a);
    native public static float atan(float a);
    native public static float atan2(float a, float b);
    native public static float ceil(float a);
    native public static float sqrt(float a);
    native public static float exp(float a);
    native public static float floor(float a);
    native public static float log(float a);
    native public static float pow(float a, float b);
    native public static float rint(float a);

    public static int round(float a)
    {
        if ((a < (float)Integer.MIN_VALUE) || (a == Float.NEGATIVE_INFINITY))
        {
            return Integer.MIN_VALUE;
        }
        else if ((a > (float)Integer.MAX_VALUE) || (a == Float.POSITIVE_INFINITY))
        {
            return Integer.MAX_VALUE;
        }
        return (int)rint(a);
    }
*/
}
