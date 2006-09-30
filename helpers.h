/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

namespace kLock
{
	//funkcja wywo³uj¹ca akcjê
	void CallAction(sUIAction act);

	//funkcja wywo³uj¹ca akcjê
	void CallAction(int parent, int id, int cnt = AC_CURRENT);

	//funkcja do subclassingu
	int SubclassAction(int group, int id, int mask = UIAIM_ALL);

	//funkcja sprawdzaj¹ca, czy plugin o danym necie istnieje
	int PluginExists(int net, int type = IMT_ALL);

	//funkcja sprawdzaj¹ca, czy plugin o danym sigu istnieje
	int PluginExists(std::string sig, int type = IMT_ALL);
}