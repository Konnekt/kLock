namespace kLock
{
	// funkcja wywo�uj�ca akcj�
	void CallAction(sUIAction act);

	// funkcja wywo�uj�ca akcj�
	void CallAction(int parent, int id, int cnt = AC_CURRENT);

  // funkcja do subclassingu
  int SubclassAction(int group, int id, int mask = UIAIM_ALL);
}