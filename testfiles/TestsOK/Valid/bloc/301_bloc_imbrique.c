int main()
{
    int a;
    {
        a = 9;
        int b = 0;
        {
            a = b;
            int c;
        }
    }
    return a;
}