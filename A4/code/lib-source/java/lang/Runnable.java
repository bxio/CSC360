/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Must be implemented by classes that will run in a separate threads.
 *
 * For detailed description of this class see the Java language documentation.
 */
public interface Runnable
{
    /**
     * This method must be overwritten in a class that implements this interface.
     */
    public abstract void run();
}
