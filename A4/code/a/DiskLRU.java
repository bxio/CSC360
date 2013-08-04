import java.util.*;
import java.lang.*;
import uvic.posix.*;
import uvic.disk.*;

/**
	

*/
public class DiskLRU extends DiskFCFS
{
	private static final int CACHE_SIZE = 16;
	public DiskLRU(int startPosition)
	{
		super(startPosition);
	}
}
