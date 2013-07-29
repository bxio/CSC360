/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;


/**
 * Thrown when an thread was interrupted during sleep or wait.
 *
 * For detailed description of this class see the Java language documentation.
 */
public class InterruptedException extends Exception
{
    public InterruptedException ()
    {
        super();
    }

    public InterruptedException (String s)
    {
        super(s);
    }
}
