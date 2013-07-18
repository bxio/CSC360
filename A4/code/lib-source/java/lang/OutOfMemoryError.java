/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown if the Java VM runs out of the memory.
 *
 * The following reasons can cause this exception to be thrown:
 *   1. NoHeap - no more memory on the heap
 *   2. NoRoots - no more memory for the object references
 *   3. NoFrames - no more memory for method frames
 *   4. StrTableFull - String objects reference table is full
  *
 */
public class OutOfMemoryError extends VirtualMachineError
{
    /**
     * Creates an OutOfMemoryError object.
     */
    public OutOfMemoryError()
    {
        super();
    }

    /**
     * Creates an OutOfMemoryError with the detail message.
     */
    public OutOfMemoryError(String s)
    {
        super(s);
    }
}
