/*
 * License - http://www.gnu.org/licenses/lgpl.txt
 */

namespace kLock
{
	//funkcja wywo�uj�ca akcj�
	void callAction(sUIAction act);

	//funkcja wywo�uj�ca akcj�
	void callAction(int parent, int id, int cnt = AC_CURRENT);

	//funkcja do subclassingu
	int subclassAction(int group, int id, int mask = UIAIM_ALL);

	//funkcja sprawdzaj�ca, czy plugin o danym necie istnieje
	int pluginExists(int net, int type = IMT_ALL);

	//funkcja sprawdzaj�ca, czy plugin o danym sigu istnieje
	int pluginExists(std::string sig, int type = IMT_ALL);
}