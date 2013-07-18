/*
** simpleRTJ java language core library component.
**
** See the http://www.gnu.org/copyleft/gpl.html for
** details on usage and redistribution of this file.
*/
package java.lang;

/**
 * Encapsulates <strong>char</strong> java primitive type.
 * Implements only an ASCII subset.
 *
 * For detailed description of this class see the Java language documentation.
 */
public final class Character extends Object {

    public static final int MIN_RADIX = 2;
    public static final int MAX_RADIX = 36;
    public static final char MIN_VALUE = '\u0000';
    public static final char MAX_VALUE = '\uffff';
    public static final int UNASSIGNED = 0;
    public static final int UPPERCASE_LETTER = 1;
    public static final int LOWERCASE_LETTER = 2;
    public static final int TITLECASE_LETTER = 3;
    public static final int MODIFIER_LETTER = 4;
    public static final int OTHER_LETTER = 5;
    public static final int NON_SPACING_MARK = 6;
    public static final int ENCLOSING_MARK = 7;
    public static final int COMBINING_SPACING_MARK = 8;
    public static final int DECIMAL_DIGIT_NUMBER = 9;
    public static final int LETTER_NUMBER = 10;
    public static final int OTHER_NUMBER = 11;
    public static final int SPACE_SEPARATOR = 12;
    public static final int LINE_SEPARATOR = 13;
    public static final int PARAGRAPH_SEPARATOR = 14;
    public static final int CONTROL = 15;
    public static final int FORMAT = 16;
    public static final int PRIVATE_USE = 18;
    public static final int SURROGATE = 19;
    public static final int DASH_PUNCTUATION = 20;
    public static final int START_PUNCTUATION = 21;
    public static final int END_PUNCTUATION = 22;
    public static final int CONNECTOR_PUNCTUATION = 23;
    public static final int OTHER_PUNCTUATION = 24;
    public static final int MATH_SYMBOL = 25;
    public static final int CURRENCY_SYMBOL = 26;
    public static final int MODIFIER_SYMBOL = 27;
    public static final int OTHER_SYMBOL = 28;

    private char value;

    public Character(char value)
    {
        this.value = value;
    }

    public char charValue()
    {
        return value;
    }

    public int hashCode()
    {
        return (int)value;
    }

    public boolean equals(Object obj)
    {
        if (obj instanceof Character)
        {
            return (charValue() == ((Character)obj).charValue());
        }
        else
        {
            return false;
        }
    }

    public String toString()
    {
        return String.valueOf(value);
    }

    public static boolean isSpace(char ch)
    {
        return isWhitespace(ch);
    }

    public static boolean isLetterOrDigit(char ch)
    {
        return isJavaIdentifierPart(ch);
    }

    public static boolean isJavaLetter(char ch)
    {
        return isJavaIdentifierStart(ch);
    }

    public static boolean isJavaLetterOrDigit(char ch)
    {
        return isJavaIdentifierPart(ch);
    }

    public static boolean isTitleCase(char ch)
    {
        return (ch == 0x01C5 || ch == 0x01C8 || ch == 0x01CB || ch == 0x01F2);
    }

    public static char toTitleCase(char ch)
    {
        if (ch == 0x01C6)
        {
            return 0x01C6;
        }
        else if (ch == 0x01C9)
        {
            return 0x01C8;
        }
        else if (ch == 0x01CC)
        {
            return 0x01CB;
        }
        else if (ch == 0x01F3)
        {
            return 0x01F2;
        }

        return ch;
    }

    public static boolean isIdentiferIgnored(char ch)
    {
        if ((ch >= '\u0000' && ch <= '\u0008') ||
            (ch >= '\u000E' && ch <= '\u001B') ||
            (ch >= '\u007F' && ch <= '\u009F') ||
            (ch >= '\u200C' && ch <= '\u200F') ||
            (ch >= '\u200A' && ch <= '\u200E') ||
            (ch >= '\u206A' && ch <= '\u206F') ||
            (ch == '\uFEFF'))
        {
            return true;
        }

        return false;
    }

    public static boolean isLowerCase(char ch)
    {
        return (ch >= 'a' && ch <= 'z');
    }

    public static boolean isUpperCase(char ch)
    {
        return (ch >= 'A' && ch <= 'Z');
    }

    public static boolean isDigit(char ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    public static boolean isLetter(char ch)
    {
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
    }

    public static char toLowerCase(char ch)
    {
        char result=ch;

        if (isUpperCase(ch))
        {
            result = (char)(((int )ch) + 32);
        }

        return result;
    }

    public static char toUpperCase(char ch)
    {
        char result=ch;

        if (isLowerCase(ch))
        {
            result = (char)(((int )ch) - 32);
        }

        return result;
    }

    public static int digit(char ch, int radix)
    {
        if (radix < MIN_RADIX || radix > MAX_RADIX)
        {
            return -1;
        }

        int val = radix;
        if (isDigit(ch))
        {
            val = ch - '0';
        }
        else if (isLetter(ch))
        {
            val = ((int)toLowerCase(ch)) - ((int)'a') + 10;
        }

        if (val < radix)
        {
            return val;
        }

        return -1;
    }

    public static char forDigit(int digit, int radix)
    {
        if ((radix<MIN_RADIX) || (radix>MAX_RADIX))
        {
            return '\u0000';
        }
        if (digit > radix)
        {
            return '\u0000';
        }
        else if (digit < 10)
        {
            return (char )(((int )'0')+digit);
        }

        return (char )(((int )'a')+(digit-10));
    }

    public static int getNumericValue(char ch)
    {
        return digit(ch, 10);
    }

    public static int getType(char ch)
    {
        return UNASSIGNED;
    }

    public static boolean isISOControl(char ch)
    {
        if ((ch >= '\u0000' && ch <= '\u001F') ||
            (ch >= '\u007F' && ch <= '\u009F'))
        {
            return true;
        }

        return false;
    }

    public static boolean isJavaIdentifierPart(char ch)
    {
        return (isLetter(ch) || ch == '$' || ch == '_' || isDigit(ch));
    }

    public static boolean isJavaIdentifierStart(char ch)
    {
        return (isLetter(ch) || ch == '$' || ch == '_');
    }

    public static boolean isSpaceChar(char ch)
    {
        if (ch == 0x0009 || ch == 0x000a || ch == 0x0000c || ch == 0x000d || ch == 0x0020)
        {
            return true;
        }

        return false;
    }

    public static boolean isUnicodeIdentifierPart(char ch)
    {
        return isJavaIdentifierPart(ch);
    }

    public static boolean isUnicodeIdentifierStart(char ch)
    {
        return isJavaIdentifierStart(ch);
    }

    public static boolean isWhitespace(char ch)
    {
        if (ch == 0x0009 || ch == 0x000A || ch == 0x000B || ch == 0x000C || ch == 0x000D ||
            ch == 0x001C || ch == 0x001D || ch == 0x001F)
        {
            return true;
        }
        else if (ch == ' ')
        {
          return true;
        }

        return false;
    }
}

