/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Thrown when unexpected internal error has occurred in
 * the Java Virtual Machine.
 *
 * InternalError can be thrown when one of the following occur:
 *     1.  something went wrong; see detail code<br>
 *     2.  VM run out of memory; see detail code<br>
 *     3.  missing method definition<br>
 *     4.  method is abstract - no code present<br>
 *     5.  null pointer discovered<br>
 *     6.  array index is out of bound for this array<br>
 *     7.  unable to store data type in this array<br>
 *     8.  arithmetic error occured<br>
 *     9.  unable to cast object to specified type<br>
 *     10. specified array size has negative value<br>
 *     11. thrown when current thread is not owner on object lock<br>
 */
public class InternalError extends VirtualMachineError
{
    /**
     * Creates an InternalError class.
     */
    public InternalError()
    {
        super();
    }

    /**
     * Creates an InternalError with the detail message.
     */
    public InternalError(String s)
    {
        super(s);
    }
}
