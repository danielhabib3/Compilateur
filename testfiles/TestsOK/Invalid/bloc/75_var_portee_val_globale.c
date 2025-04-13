int main()
{
    int a;
    a=4;
    {
        int a;
        a=2;
        {
            int b = 3;
            a=3;
        }
        a = b;
    }

    return b;
}