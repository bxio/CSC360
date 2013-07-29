/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * This class is not used by the Java VM. Is here because it is a predefined class.
 */
public class LinkageError extends Error
{
    public LinkageError()
    {
        super();
    }

    public LinkageError(String s)
    {
        super(s);
    }
}
