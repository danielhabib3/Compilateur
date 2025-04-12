int main()
{
    int a=1;
    {
        int a=2;
        {
            int a;
            {
                a=3;
            }
        }
    }
    return a;
}