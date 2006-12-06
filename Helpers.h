/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

namespace kLock
{
	//funkcja wywo³uj¹ca akcjê
	void callAction(sUIAction act);

	//funkcja wywo³uj¹ca akcjê
	void callAction(int parent, int id, int cnt = AC_CURRENT);

	//funkcja sprawdzaj¹ca, czy plugin o danym necie istnieje
	int pluginExists(int net, int type = IMT_ALL);

	//funkcja sprawdzaj¹ca, czy plugin o danym sigu istnieje
	int pluginExists(std::string sig, int type = IMT_ALL);
}